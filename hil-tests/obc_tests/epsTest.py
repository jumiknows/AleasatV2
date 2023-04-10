# The following imports are required
from obc_test_internals import obc_test, timeout

"""
You want to inherit the Setup and Teardown of OBCTest so add it as the Parent Class.
Should you want to change the default SetUp or tearDown you can implement it here.
"""

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
        resp = self.obc.send_cmd("TEST_EPS_READ_FLOAT", 1) # 1 = v_battery in volts
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["eps_err"], 0)
    
    @timeout.timeout(10)
    def test_eps_read_float_raw_battery_voltage(self):
        resp = self.obc.send_cmd("TEST_EPS_READ_FLOAT_RAW", 1) # 1 = v_battery in volts
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["eps_err"], 0)

    @timeout.timeout(10)
    def test_eps_read_int_input_conditions(self):
        resp = self.obc.send_cmd("TEST_EPS_READ_INT", 23) # 23 = input conditions bitfield
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["eps_err"], 0)

    @timeout.timeout(10)
    def test_eps_write_vbatt_en(self):
        resp = self.obc.send_cmd("TEST_EPS_WRITE", 1, 1) # write vbatt_en = AUTO_ON
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["eps_err"], 0)

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(EpsTest)
