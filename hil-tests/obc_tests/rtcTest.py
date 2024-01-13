"""
Tests to verify RTC functionalities.
"""
# The following imports are required
from obc_test_internals import obc_test, timeout

import numpy as np

class RtcTest(obc_test.OBCTest):

    # Set time to 1 January 2020, 00:00:00. Wait 3 seconds and then get time.
    # SUCCESS if time read is 1 January 2020, 00:00:03.
    #
    # Seconds to wait can range from 0 to 255. 
    @timeout.timeout(5)
    def test_set_get_time(self):
        resp = self.obc.send_cmd("TEST_RTC_GET_TIME", 3) # 3 = duration_s. Seconds to wait. 
        self.assertTrue(resp.is_success)

    # Get current time, set alarm for 3 seconds from current time, await 4 seconds for alarm. 
    # SUCCESS if alarm interrupt has been produced and handled before the 4-second wait is up.
    #
    # Alarm can be set from 1 to 255 in the future.
    @timeout.timeout(5)
    def test_alarm_in_n_sec(self):
        resp = self.obc.send_cmd("TEST_RTC_ALARM_N_SEC", 3) # 3 = duration_s. Alarm to go off duration_s into the test.
        self.assertTrue(resp.is_success)

    # Get current time, wait for 3 seconds, capture the timestamp and compare it against the initial time. 
    # SUCCESS if the timestamp records a time 3 seconds after the initial time.
    #
    # Seconds to wait can range from 0 to 255.
    @timeout.timeout(5)
    def test_cap_and_get_timestamp(self):
        resp = self.obc.send_cmd("TEST_RTC_CAP_AND_GET_TIMESTAMP", 3) # 3 = duration_s. Seconds to wait.
        self.assertTrue(resp.is_success)
    

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(RtcTest)
