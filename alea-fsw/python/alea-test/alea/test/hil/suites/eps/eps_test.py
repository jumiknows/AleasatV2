from alea.test import timeout
from alea.test.hil import hil_test

class EPSTest(hil_test.HILTest):

    def setUp(self) -> None:
        # TODO: Add EPS emulator to HIL setup
        self.skipTest("EPS not available")

    @timeout.timeout(10)
    def test_eps_read_float_battery_voltage(self):
        resp = self.ttc.send_obc_cmd("TEST_EPS_READ_FLOAT", 1) # 1 = v_battery in volts
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["eps_err"], 0)
    
    @timeout.timeout(10)
    def test_eps_read_float_raw_battery_voltage(self):
        resp = self.ttc.send_obc_cmd("TEST_EPS_READ_FLOAT_RAW", 1) # 1 = v_battery in volts
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["eps_err"], 0)

    @timeout.timeout(10)
    def test_eps_read_int_input_conditions(self):
        resp = self.ttc.send_obc_cmd("TEST_EPS_READ_INT", 23) # 23 = input conditions bitfield
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["eps_err"], 0)

    @timeout.timeout(10)
    def test_eps_write_vbatt_en(self):
        resp = self.ttc.send_obc_cmd("TEST_EPS_WRITE", 1, 1) # write vbatt_en = AUTO_ON
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["eps_err"], 0)

if __name__ == '__main__':
    hil_test.main(EPSTest)
