# The following imports are required
from obc_test_internals import obc_test, timeout

"""
You want to inherit the Setup and Teardown of OBCTest so add it as the Parent Class.
Should you want to change the default SetUp or tearDown you can implement it here.
"""

passKeyword = "PASS"
failKeyword = "FAIL"

class EpsTest(obc_test.OBCTest):

    # Timeout is in seconds, it will also record how long this test takes.
#    @timeout.timeout(10)
#    def test_eps_read_sanity(self):
#        self.obc.send("test_eps_read_sanity")
#        while 1:
#            msg = self.obc.recv()
#            if passKeyword in msg.payload:
#                return

#    @timeout.timeout(10)
#    def test_eps_writeread_sanity(self):
#        self.obc.send_q.put("0 test_eps_writeread_sanity * *")
#        while 1:
#            msg = self.obc.recv()
#            if passKeyword in msg.payload:
#                return

    @timeout.timeout(10)
    def test_eps_read_float_battery_voltage(self):
        self.obc.send("test_eps_read_float", 1) # 1 = v_battery in volts
        self.wait_for_keyword("tx: 0")
    
    @timeout.timeout(10)
    def test_eps_read_float_raw_battery_voltage(self):
        self.obc.send("test_eps_read_float_raw", 1) # 1 = v_battery in volts
        self.wait_for_keyword("tx: 0")

    @timeout.timeout(10)
    def test_eps_read_int_input_conditions(self):
        self.obc.send("test_eps_read_int", 23) # 23 = input conditions bitfield
        self.wait_for_keyword("tx: 0")

    @timeout.timeout(10)
    def test_eps_write_vbatt_en(self):
        self.obc.send("test_eps_write", 1, 1) # write vbatt_en = AUTO_ON
        self.wait_for_keyword("tx: 0")

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(EpsTest)
