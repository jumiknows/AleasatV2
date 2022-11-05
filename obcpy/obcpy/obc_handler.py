from queue import Queue, Empty
from threading import Thread
import datetime
from typing import Callable
import time

from .serial_handler import SerialHandler
from .obc_message import OBCMessage

class OBCHandler:
    def __init__(self):
        self._tx_q = Queue()
        self._rx_q = Queue()

        self._serial_handler = SerialHandler(self._tx_q, self._rx_q, tx_callback=self.tx_callback)

        self._tx_thread: Thread = None
        self._rx_thread: Thread = None

        self.tx_callback_msg = ""
        self.tx_callback_flag = False

    @property
    def connected(self) -> bool:
        return self._serial_handler.connected

    def start(self, serial_port: str) -> bool:
        if self._serial_handler.connect(serial_port):
            print("Starting OBC RX and TX threads")

            self._tx_thread = Thread(target=self._serial_handler.run_tx, daemon=True)
            self._rx_thread = Thread(target=self._serial_handler.run_rx, daemon=True)

            self._rx_thread.start()
            self._tx_thread.start()
            return True
        return False

    def stop(self):
        if (self._tx_thread is not None) and (self._rx_thread is not None):
            print("Stopping OBC RX and TX threads")
            self._serial_handler.stop()
            self._tx_thread.join()
            self._rx_thread.join()

            self._serial_handler.disconnect()

            self._tx_thread = None
            self._rx_thread = None

    def send_raw(self, msg: str, block: bool = False):
        # if not self.connected:
        #     return

        if block:
            # Wait until the TX queue is empty
            start = time.time()
            while not self._tx_q.empty():
                if (time.time() - start) > 2:
                    print("Timeout waiting for TX queue")
                    return

            # Mark the message we're waiting for
            self.tx_callback_msg = msg
            self.tx_callback_flag = False

        self._tx_q.put(msg)

        if block:
            # Wait until the message has been sent
            start = time.time()
            while not self.tx_callback_flag:
                if (time.time() - start) > 2:
                    print("Timeout waiting for TX")
                    return

    def send(self, cmd: str, *args, block: bool = False, seq_num: int = 0, when: datetime.datetime = None):
        if when is None:
            when_str = "* *"
        else:
            # TODO check format
            when_str = when.strftime("%Y-%m-%d %H:%M:%S")

        args_str = " ".join([str(arg) for arg in args])

        msg = f"{str(seq_num)} {cmd} {when_str} {args_str}"
        self.send_raw(msg, block=block)

    def ping(self, block: bool = False):
        self.send("ping", block=block)

    def reset(self):
        self.send("nrst", block=True)
        self._serial_handler.reset_rx()

    def set_current_datetime(self, block: bool = False):
        now = datetime.datetime.now()
        self.send("set_time", "a", (now.year - 2000), now.month, now.day, now.hour, now.minute, now.second, block=block)

    def recv(self, timeout: float = None) -> OBCMessage:
        try:
            return self._rx_q.get(block=True, timeout=timeout)
        except Empty:
            return None

    def tx_callback(self, msg: str):
        if self.tx_callback_msg == msg:
            self.tx_callback_flag = True
