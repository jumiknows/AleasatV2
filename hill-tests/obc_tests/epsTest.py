# The following imports are required
from obc_test_internals import OBCTestClass, timeout
import argparse, unittest, time, datetime

"""
You want to inherit the Setup and Teardown of OBCTest so add it as the Parent Class.
Should you want to change the default SetUp or tearDown you can implement it here.
"""

passKeyword = "PASS"
failKeyword = "FAIL"

class EpsTest(OBCTestClass.OBCTest):

    # Timeout is in seconds, it will also record how long this test takes.
#    @timeout.timeout(10)
#    def test_eps_read_sanity(self):
#        self.obc.send_q.put("0 test_eps_read_sanity * *")
#        while 1:
#            msg = self.obc.recv_q.get(block=True)
#            if passKeyword in msg.print():
#                return

#    @timeout.timeout(10)
#    def test_eps_writeread_sanity(self):
#        self.obc.send_q.put("0 test_eps_writeread_sanity * *")
#        while 1:
#            msg = self.obc.recv_q.get(block=True)
#            if passKeyword in msg.print():
#                return

    @timeout.timeout(10)
    def test_eps_read_float_battery_voltage(self):
        self.obc.send_q.put("0 test_eps_read_float * * 1") # 1 = v_battery in volts
        while 1:
            msg = self.obc.recv_q.get(block=True)
            if "tx: 0" in msg.print():
                return

    @timeout.timeout(10)
    def test_eps_read_int_input_conditions(self):
        self.obc.send_q.put("0 test_eps_read_int * * 23") # 23 = input conditions bitfield
        while 1:
            msg = self.obc.recv_q.get(block=True)
            if "tx: 0" in msg.print():
                return

    @timeout.timeout(10)
    def test_eps_write_vbatt_en(self):
        self.obc.send_q.put("0 test_eps_write * * 1 1") # write vbatt_en = AUTO_ON
        while 1:
            msg = self.obc.recv_q.get(block=True)
            if "tx: 0" in msg.print():
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

    EpsTest.PORT = args["port"]
    try:
        unittest.main()
    except Exception as e:
        print("Test timed out!")
