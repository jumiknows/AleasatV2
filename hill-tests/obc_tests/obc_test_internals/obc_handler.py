import queue, threading
from obc_test_internals import ports


class OBCHandler:
    recv_q = None
    send_q = None
    serial_handler = None
    connect_thread = None

    def __init__(self):
        self.recv_q = queue.Queue()
        self.send_q = queue.Queue()
        self.serial_handler = ports.SerialHandler(self.send_q, self.recv_q)

    def connect(self, port: str):
        return self.serial_handler.connect(port)

    def start(self):
        self.connect_thread = threading.Thread(target=self.serial_handler.run)
        self.connect_thread.start()

    def stop(self):
        self.connect_thread.join(timeout=1)
        self.serial_handler.close()
