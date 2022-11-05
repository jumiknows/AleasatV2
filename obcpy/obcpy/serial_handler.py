from typing import Callable
from queue import Queue, Empty
from threading import Event

import serial

from .obc_message import OBCMessageParser

class SerialHandler:
    def __init__(self, tx_q: Queue, rx_q: Queue, tx_callback: Callable[[str], None] = None):
        self._tx_q = tx_q
        self._rx_q = rx_q

        self._tx_callback = tx_callback

        self._stop_event = Event()

        self._message_parser = OBCMessageParser()

        self._ser: serial.Serial = None

    def __del__(self):
        self.disconnect()

    @property
    def connected(self) -> bool:
        return (self._ser is not None) and (self._ser.is_open)

    def connect(self, serial_port: str) -> bool:
        try:
            print(f"Connecting to OBC on port: {serial_port}")
            ser = serial.Serial(
                port     = serial_port,
                baudrate = 115200,
                timeout  = 1,
                parity   = serial.PARITY_NONE,
                stopbits = serial.STOPBITS_ONE,
                bytesize = serial.EIGHTBITS
            )
            if not ser.is_open:
                ser.open()

            self._ser = ser
            self._stop_event.clear()
            self.reset_rx()
            return True

        except Exception as e:
            print(e)
            return False

    def disconnect(self):
        if self._ser is not None:
            print(f"Disconnecting from OBC")
            self._ser.close()
            self._ser = None

    def stop(self):
        self._stop_event.set()

    def reset_rx(self):
        if self._ser is not None:
            self._ser.reset_input_buffer()
        self._message_parser.reset()

    def run_tx(self):
        while not self._stop_event.is_set():
            if self.connected:
                try:
                    tx_msg = self._tx_q.get(block=True, timeout=0.1)
                    tx_msg_enc = f"{str(tx_msg)}\r".encode('ascii')
                    print(f"[--> OBC] {tx_msg}")

                    try:
                        self._ser.write(tx_msg_enc)
                        if self._tx_callback is not None:
                            self._tx_callback(tx_msg)
                    except Exception as e:
                        self.disconnect()
                        print(f"[ERROR] Serial TX Failed: {e}")
                except Empty:
                    pass

    def run_rx(self):
        while not self._stop_event.is_set():
            exception_count = 0
            if self.connected:
                try:
                    rx_data = self._ser.read()
                    exception_count = 0
                    if rx_data:
                        try:
                            rx_msgs = self._message_parser.push_bytes(rx_data)
                            for rx_msg in rx_msgs:
                                print(f"[<-- OBC] {rx_msg}")
                                self._rx_q.put(rx_msg)

                        except Exception as e:
                            print(f"[ERROR] Message Parse Failed: {e}")
                except Exception as e:
                    exception_count += 1
                    # Wait for 5 consecutive exceptions before giving up
                    if exception_count == 5:
                        self.disconnect()
                        print(f"[ERROR] Serial RX Failed: {e}")
