import datetime

from obcpy.interface.obc_serial_interface import OBCSerialInterface

class OBCHandler:
    def __init__(self):
        self.interface = OBCSerialInterface()

    @property
    def connected(self) -> bool:
        return self.interface.connected

    def start(self, serial_port: str) -> bool:
        return self.interface.start(serial_port)

    def stop(self):
        return self.interface.stop()

    def send_raw(self, msg: str, block: bool = False):
        self.interface.protocol.send_cmd(f"{msg}\r")

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

    def set_current_datetime(self, block: bool = False):
        now = datetime.datetime.now()
        self.send("set_time", "a", (now.year - 2000), now.month, now.day, now.hour, now.minute, now.second, block=block)
