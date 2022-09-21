from obc_test_internals import OBCTestClass, timeout
import argparse, unittest, serial, threading, time, queue

class GpsMock():
    ser = None
    def connect(self, serial_port: str) -> bool:
        try:
            self.ser = serial.Serial(
                port=serial_port,
                baudrate=9600,
                timeout=0,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS
            )

            return True

        except Exception as e:
            return False    

    def run(self):
        try:
            while 1:
                if not self.ser.is_open:
                    raise Exception("GPS Port Closed!")
                
                while self.ser.in_waiting > 0:
                    time.sleep(0.001)
                    msg = self.ser.readline()
                    while b'\r\n' not in msg:
                        time.sleep(0.001)
                        temp = self.ser.readline()
                        if temp:
                            msg+=temp
                    print(msg)
                    self.gps_recv_q.put(msg)
                    print("bleh")

                    if not self.ser.is_open:
                        return

        except Exception as e:
            return


class GpsTestClass(OBCTestClass.OBCTest):
    gps = None
    connect_thread = None
    gps_recv_q = None
    gps_send_q = None
    def setUp(self) -> None:
        super().setUp()
        self.gps_recv_q = queue.Queue()
        self.gps_send_q = queue.Queue()
        self.gps = GpsMock()
        if self.gps.connect("COM3"):
            print("SUCCESS: Connected through GPS port")
        else:
            print("ERROR: Could not connect to GPS port")
        self.connect_thread = threading.Thread(target=self.gps.run)
        self.connect_thread.start()
    
    def tearDown(self) -> None:
        self.connect_thread.join(timeout=1)
        self.gps.ser.close()

class GpsTest(GpsTestClass):

    @timeout.timeout(10)
    def test_gps_restart(self):
        self.obc.send_q.put("0 restart_gps * *")
        while 1:
            print(self.gps_recv_q)
            msg_b = self.gps_recv_q.get(block=True)
            msg_b.hex()
            print(msg)
            if msg[0:4] == 'a0a1' and msg[4:8] == '000f' \
                and msg[8:10] == '01' and msg[10:12] == '03' \
                and msg[12:16] == '07e7' and msg[16:18] == '01' \
                and msg[18:20] == '01' \
                and msg[26:38] == 'b1c4e11000df' \
                and msg[40:] == '0d0a':
                return

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--listports", default="False")
    parser.add_argument("--port", default="COM8")
    args = vars(parser.parse_args())

    if args["listports"] == "True":
        print(OBCTestClass.get_serial_ports())
        exit(0)

    GpsTest.PORT = args["port"]
    try:
        unittest.main()
    except Exception as e:
        print("Test timed out!")
