"""
Test the read and write functions of the GPS driver
"""
# The following imports are required
from obc_test_internals import obc_test, timeout
import time

"""
You want to inherit the Setup and Teardown of OBCTest so add it as the Parent Class.
Should you want to change the default SetUp or tearDown you can implement it here.
"""
class GpsTest(obc_test.OBCTest):

    @timeout.timeout(20)
    def test_query_sw_ver(self):
        resp = self.obc.send_cmd("GPS_QUERY_SW_VER")
        self.assertTrue(resp.is_success)
        self.assertEqual(resp["gps_software_version"], [2,1,5,0,1,7,15,1,27])

    @timeout.timeout(20)
    def test_power_mode(self):
        # Set it to power save.
        resp = self.obc.send_cmd("GPS_CONFIGURE_POWER_MODE", 0x02, True)
        self.assertTrue(resp.is_success)
        time.sleep(0.5)
        # Get the power mode
        resp = self.obc.send_cmd("GPS_QUERY_POWER_MODE")
        self.assertTrue(resp.is_success)

    @timeout.timeout(20)
    def test_query_crc_info(self):
        resp = self.obc.send_cmd("GPS_QUERY_SW_CRC")
        self.assertTrue(resp.is_success)
        print(resp["software_crc"])

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(GpsTest)
