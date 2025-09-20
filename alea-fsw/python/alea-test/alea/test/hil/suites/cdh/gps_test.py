import time

from alea.test import timeout
from alea.test.hil import hil_test

class GPSTest(hil_test.HILTest):

    def setUp(self) -> None:
        # TODO: Add GPS (emulator?) to HIL setup
        self.skipTest("GPS not available")

    @timeout.timeout(20)
    def test_query_sw_ver(self):
        resp = self.ttc.send_obc_cmd("GPS_QUERY_SW_VER")
        self.assertTrue(resp.is_success)
        self.assertEqual(resp["gps_software_version"], [2,1,5,0,1,7,15,1,27])

    @timeout.timeout(20)
    def test_power_mode(self):
        # Set it to power save.
        resp = self.ttc.send_obc_cmd("GPS_CONFIGURE_POWER_MODE", 0x02, True)
        self.assertTrue(resp.is_success)
        time.sleep(0.5)
        # Get the power mode
        resp = self.ttc.send_obc_cmd("GPS_QUERY_POWER_MODE")
        self.assertTrue(resp.is_success)

    @timeout.timeout(20)
    def test_query_crc_info(self):
        resp = self.ttc.send_obc_cmd("GPS_QUERY_SW_CRC")
        self.assertTrue(resp.is_success)
        print(resp["software_crc"])

if __name__ == '__main__':
    hil_test.main(GPSTest)
