import serial, queue
from obc_test_internals import alea_message


class SerialHandler():
    send_q = None
    recv_q = None
    ser = None
    message_assistant = None

    def __init__(self, send_q: queue.Queue, recv_q: queue.Queue):
        self.send_q = send_q
        self.recv_q = recv_q
        self.message_assistant = alea_message.AleaMessageParser()

    def close(self):
        self.ser.close()

    def connect(self, serial_port: str) -> bool:
        try:
            ser = serial.Serial(
                port=serial_port,
                baudrate=115200,
                timeout=5,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS
            )
            if not ser.is_open:
                ser.open()

            self.ser = ser

            return True

        except Exception as e:
            return False

    def run(self):
        try:
            while 1:

                if not self.ser.is_open:
                    raise Exception("Serial Port Closed!")

                if not self.send_q.empty():
                    s = self.send_q.get()
                    #print("-> " + s)
                    self.ser.write(bytes(s + '\r', 'ascii'))

                while self.ser.in_waiting > 0:
                    out = self.ser.readline()
                    if out:
                        msgs = self.message_assistant.parse(out)
                        for msg in msgs:
                            #print(msg.print())
                            self.recv_q.put(msg)

                    if not self.ser.is_open:
                        # were done!
                        return
        except Exception as e:
            return

