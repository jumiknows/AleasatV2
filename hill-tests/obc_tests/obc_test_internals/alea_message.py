import time, datetime, struct

lvl_map = {
    0: "Debug",
    1: "Warning",
    2: "Info",
    3: "Error",
}

TIMESTAMPS_ON = False
FUNCTION_ID_ON = False
FUNC_ID_ON = False
MSG_LVL_ON = False


class AleaMessage():
    """
    Alea OBC header is
    - 7 bytes long
    - 0-4th byte is time in epoch
    - 5th byte, lesser two bits are level, higher six are function id
    - 6th byte, lesser four bits are function id, higher four bits are padding
    - 7th byte: length of message
    """
    msg_time = None
    func_id = None
    msg_len = None
    msg_lvl = None
    msg = None

    def __init__(self, msg_len: int, msg: str, msg_time: datetime.datetime, msg_lvl: int, func_id: int):
        self.msg_len = msg_len
        self.msg = msg
        self.msg_time = msg_time
        self.msg_lvl = msg_lvl
        self.func_id = func_id

    def print(self, timestamp=False):
        msg = self.msg

        if timestamp:
            return msg, self.msg_time

        """
        if MSG_LVL_ON:
            msg = lvl_map[self.msg_lvl] + " " + msg
        if TIMESTAMPS_ON:
            msg = "[" + self.msg_time.strftime("%Y-%m-%d %H:%M:%S") + "] " + msg
        if FUNC_ID_ON:
            msg = "[ID: " + str(self.func_id) + "] " + msg
        if MSG_LVL_ON:
            msg = "[" + lvl_map[self.msg_lvl] + "] " + msg
        """
        return msg


class AleaMessageParser():
    start_time = None

    def __init__(self):
        self.start_time = datetime.datetime.now()

    def parse(self, msg: bytes):
        #print(msg[0:4].hex())

        start = datetime.datetime.strptime('20-1-1 0:0:0', '%y-%m-%d %H:%M:%S')
        msg_time = datetime.datetime.fromtimestamp(start.timestamp() + int.from_bytes(msg[0:4], "little"))
        msg_lvl = int(msg[4] & 0b00000011)
        func_id = int((msg[4] & 0b11111100) | (msg[5] & 0b00001111))
        msg_len = int(msg[6])
        try:
            msg_txt = str(msg[7:7 + msg_len], 'ascii')

            # temporary hack
            if " >" in msg_txt:
                msg_txt = msg_txt[2:]
        except Exception as e:
            print("It went wrong!")
            print(msg)
            return []

        alea_msg = AleaMessage(msg_len, msg_txt, msg_time, msg_lvl, func_id)
        total_len = 7 + msg_len
        if len(msg) > 7 + msg_len:
            # This is getting ridiculous
            # now theres two of them!
            return [alea_msg] + self.parse(msg[total_len:])

        return [alea_msg]
