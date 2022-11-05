from typing import List
import datetime

class OBCMessage:
    """
    OBC header is
    - 7 bytes long
    - 0-4th byte is time in epoch
    - 5th byte, lesser two bits are level, higher six are function id
    - 6th byte, lesser four bits are function id, higher four bits are padding
    - 7th byte: length of payload
    """

    LEVEL_MAP = {
        0: "Debug",
        1: "Warning",
        2: "Info",
        3: "Error",
    }

    def __init__(self):
        self.timestamp   : datetime.datetime = None
        self.level       : int               = None
        self.func_id     : int               = None
        self.payload_len : int               = None
        self.payload     : str               = None

    def __str__(self) -> str:
        line = ""
        line += f"[{OBCMessage.LEVEL_MAP[self.level]}] "
        line += f"[{self.timestamp.strftime('%Y-%m-%d %H:%M:%S')}] "
        line += f"[ID: {str(self.func_id)}] "
        line += self.payload
        return line

class OBCMessageParser():
    EPOCH_BASE = datetime.datetime(2020, 1, 1, 0, 0, 0)

    def __init__(self):
        self.start_time = datetime.datetime.now()

        self.cur_msg: OBCMessage = None
        self.cur_data = bytearray()

    def push_bytes(self, data: bytes) -> List[OBCMessage]:
        self.cur_data.extend(data)

        # Initialize
        if self.cur_msg is None:
            self.cur_msg = OBCMessage()

        # Timestamp
        if (self.cur_msg.timestamp is None) and (len(self.cur_data) >= 5):
            self.cur_msg.timestamp = datetime.datetime.fromtimestamp(self.EPOCH_BASE.timestamp() + int.from_bytes(self.cur_data[0:4], "little"))

        # Level
        if (self.cur_msg.level is None) and (len(self.cur_data) >= 5):
            self.cur_msg.level = int(self.cur_data[4] & 0b00000011)

        # Func ID
        if (self.cur_msg.func_id is None) and (len(self.cur_data) >= 6):
            self.cur_msg.func_id = int((self.cur_data[4] & 0b11111100) | (self.cur_data[5] & 0b00001111))

        # Msg Len
        if (self.cur_msg.payload_len is None) and (len(self.cur_data) >= 7):
            self.cur_msg.payload_len = int(self.cur_data[6])

        # Payload
        if (self.cur_msg.payload is None) and (self.cur_msg.payload_len is not None):
            total_len = 7 + self.cur_msg.payload_len
            if (len(self.cur_data) >= total_len):
                try:
                    payload = str(self.cur_data[7:total_len], 'ascii')

                    # temporary hack
                    if " >" in payload:
                        payload = payload[2:]

                    self.cur_msg.payload = payload
                except Exception as e:
                    print(f"Message parse error: {e}")
                    print(self.cur_data)

                    # Reset internal state
                    self.reset()
                    return []

                # Finished a message
                completed_msg = self.cur_msg

                # Clear the current message and propagate leftover data
                self.cur_msg = None
                if (len(self.cur_data) > total_len):
                    leftover_data = self.cur_data[total_len:]
                    self.cur_data = bytearray()
                    return [completed_msg] + self.push_bytes(leftover_data)
                else:
                    self.cur_data = bytearray()
                    return [completed_msg]

        return []

    def reset(self):
        self.cur_msg = None
        self.cur_data = bytearray()
