import unittest
from typing import Type, List, Union
import os
import time
import argparse
import sys
import threading
import pathlib
from unittest.util import safe_repr
import json
from types import SimpleNamespace
import xmlrunner

import datetime
import numpy as np
import grpc
import alea.obc_grpc.generated.obc_pb2_grpc as obc_pb2_grpc
import alea.obc_grpc.generated.obc_pb2 as obc_pb2
from alea.common import alea_time
from alea.ttc.cmd_sys import (
    OBCResponse,
    OBCPendingResponse,
    OBCResponseDataHeader,
    OBCResponseData,
    ResponseCode,
)

from alea.ttc.ttc import TTC
from alea.ttc.ttc_base import TTCError
from alea.ttc.protocol.generic import routing
from alea.ttc.protocol.app import app_log
from alea.obcfw.log import log_spec
from alea.ttc.device.serial_device import get_serial_ports
from alea.ttc.protocol.generic import routing_impl


def preTransformCommandArg(args, cmdSpec):
    arg = {}

    for i in range(len(args)):
        spec = cmdSpec.args[i]
        argName = spec.name
        if len(spec.array_shape) != 1:
            specType = spec.__class__.__name__
            if specType == "F32" or specType == "F64":
                matrixType = "double"
            elif specType == "S64":
                matrixType = "sint64"
            elif specType == "U64":
                matrixType = "uint64"
            elif specType.startswith("U"):
                matrixType = "uint32"
            else:
                matrixType = "sint32"
            arg[argName] = getattr(obc_pb2, f"{matrixType}_matrix")(
                rows=spec.array_shape[0],
                cols=spec.array_shape[1],
            )
            arg[argName].data.extend(
                args[i].flatten().tolist()
                if isinstance(args[i], np.ndarray)
                else [x for xs in args[i] for x in xs]
            )
            continue
        if spec.__class__.__name__ == "DateTime":
            arg[argName] = datetime.datetime.fromtimestamp(
                args[i] + alea_time.ALEADateTime.EPOCH_BASE.timestamp()
            )
        else:
            arg[argName] = args[i]
    return arg


def postTransformResponse(resp, cmdSpec):
    data = {}
    for respData in cmdSpec.resp:
        if respData.__class__.__name__ == "DateTime":
            data[respData.name] = alea_time.ALEADateTime(
                getattr(resp.data, respData.name).ToDatetime()
            )
            continue
        if len(respData.array_shape) != 1:
            data[respData.name] = np.array(
                getattr(resp.data, respData.name).data
            ).reshape(respData.array_shape)
            continue
        data[respData.name] = getattr(resp.data, respData.name)

    return OBCResponse(
        resp.cmd_inst_id,
        resp.cmd_id,
        alea_time.ALEADateTime(resp.date_time.ToDatetime()),
        resp.flags,
        OBCResponseDataHeader(
            ResponseCode(resp.data_header.code),
            alea_time.ALEADateTime(resp.data_header.exec_datetime.ToDatetime()),
        ),
        OBCResponseData(b"", data),
    )


class TTC_GRPC(TTC):
    _channel = None
    _stub: obc_pb2_grpc.obcStub | None = None
    _log_thread: threading.Thread | None = None
    _log_thread_event = threading.Event()
    _log_multi = routing_impl.MultiPacketDest[app_log.OBCLog]()

    def __init__(self):
        super().__init__(interface_type=TTC.InterfaceType.OBC_SERIAL)
        self.remove_log_listener(self._sched_resp_source)
        pass

    def start(self, port: str):
        self._channel = grpc.insecure_channel(port)
        self._stub = obc_pb2_grpc.obcStub(self._channel)
        self._log_multi.reset()
        self._log_thread_event.clear()
        self._log_thread = threading.Thread(target=self.log_listener, daemon=True)
        self._log_thread.start()

        return True

    def reset(self):
        return self.send_obc_cmd("RESET")

    def stop(self):
        self._log_thread_event.set()
        if self._log_thread is not None:
            self._log_thread.join(3)
            self._log_thread = None

        if self._channel is not None:
            self._channel.close()
            self._channel = None
            self._stub = None

    def send_obc_cmd(
        self,
        cmd_name: str,
        *args,
        date_time: alea_time.ALEADateTime = alea_time.IMMEDIATE,
        timeout: int = 10,
        progress_callback=None,
    ):
        if self._stub is None:
            raise TTCError("Not connected")

        spec = self._cmd_sys_specs.get(cmd_name)
        args = preTransformCommandArg(args, spec)

        request = getattr(obc_pb2, cmd_name.lower() + "_command_args")(
            timeout=timeout, date_time=date_time.date_time, **args
        )
        if date_time == alea_time.IMMEDIATE:
            resp = getattr(self._stub, cmd_name.lower())(request)
            if cmd_name == "RESET":
                return
            response = postTransformResponse(resp, spec)
            return response
        else:
            streamResponse = getattr(self._stub, "schedule_" + cmd_name.lower())(
                request
            )
            for r in streamResponse:
                response = r
                break

            if cmd_name == "RESET":
                return
            response = postTransformResponse(response, spec)
            response = OBCPendingResponse(response)

            def wait(timeout):
                for r in streamResponse:
                    return postTransformResponse(r, spec)

            response.wait = wait
            return response

    def log_listener(self):
        logs = self._stub.log(obc_pb2.log_args())

        try:
            for log in logs:
                obcLog = app_log.OBCLog(
                    date_time=alea_time.ALEADateTime(log.date_time.ToDatetime()),
                    log_id=log.log_id,
                    signal_id=log.signal_id,
                    payload_len=log.payload_len,
                    group_name=log.group_name,
                    group_desc=log.group_desc,
                    signal_level=log_spec.OBCLogLevel(log.signal_level),
                    signal_name=log.signal_name,
                    signal_desc=log.signal_desc,
                    data=json.loads(log.data) if log.data else None,
                )

                if self._log_thread_event.is_set():
                    break

                self._log_multi.write(obcLog)
        except grpc.RpcError:
            pass

    def add_log_listener(
        self, queue_size: int = 0
    ) -> routing.PacketSource[app_log.OBCLog]:
        bridge = routing_impl.QueuePacketBridge[app_log.OBCLog](maxsize=queue_size)
        self._log_multi.add_dest(bridge)
        return bridge

    def remove_log_listener(self, listener: routing.PacketSource[app_log.OBCLog]):
        self._log_multi.remove_dest(listener)
