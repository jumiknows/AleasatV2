import datetime
import time

from alea.test import timeout
from alea.test.hil import hil_test

class SystemTest(hil_test.HILTest):

    @timeout.timeout(5)
    def test_fw_info(self):
        resp = self.ttc.send_obc_cmd("FW_INFO")
        self.assertTrue(resp.is_success)

    @timeout.timeout(5)
    def test_get_time(self):
        timestamp = self.ttc.get_time()
        # Should be within the first 2 seconds
        self.assertLessEqual(timestamp.to_timestamp(), 2)

    @timeout.timeout(5)
    def test_set_time(self):
        target_time = datetime.datetime(2023, 7, 1, 11, 22, 33)
        resp = self.ttc.set_time(target_time)
        self.assertTrue(resp.is_success)

        obc_current_time = self.ttc.get_time()
        self.assertEqual(target_time, obc_current_time.date_time)

    @timeout.timeout(10)
    def test_schedule_command(self):
        now = self.ttc.get_time()

        # Schedule a PING command for 5 seconds in the future
        sched_time = (now + 5)
        pending_resp = self.ttc.send_obc_cmd("PING", date_time=sched_time)
        self.assertTrue(pending_resp)
        self.assertTrue(pending_resp.sched_resp.is_success_sched)

        # Wait for the response to the command
        response = pending_resp.wait(6)

        self.assertTrue(response)
        self.assertTrue(response.is_success)
        self.assertEqual(response.data_header.exec_datetime, sched_time)

    @timeout.timeout(10)
    def test_hang(self):
        self.clear_signals()
        self.ttc.send_obc_cmd("TEST_HANG", 4000000)
        self.wait_for_signal("LOG_WATCHDOG", "SW_WD_BITE", 5)

    @timeout.timeout(10)
    def test_uptime_timing2(self):
        resp = self.ttc.send_obc_cmd("GET_UPTIME") 
        time.sleep(2) # System OS waits for 2 seconds 
        resp2 = self.ttc.send_obc_cmd("GET_UPTIME")
        # GET_UPTIME difference should be 2
        self.assertEqual(resp2.data["seconds"] - resp.data["seconds"], 2)

    @timeout.timeout(30)
    def test_uptime_timing15(self):
        resp = self.ttc.send_obc_cmd("GET_UPTIME") 
        time.sleep(15) 
        resp2 = self.ttc.send_obc_cmd("GET_UPTIME")
        # GET_UPTIME difference should be 15  
        self.assertEqual(resp2.data["seconds"] - resp.data["seconds"], 15)

    @timeout.timeout(10)
    def test_uptime_reset(self):
        # RESET automatically called by test suite 
        time.sleep(5) 
        resp = self.ttc.send_obc_cmd("GET_UPTIME") 
        # RESET is inconsistent and takes a little less than 1 second
        # UPTIME should be 5 or 6
        self.assertLess(resp.data["seconds"], 7)
        self.assertGreater(resp.data["seconds"], 4)

if __name__ == '__main__':
    hil_test.main(SystemTest)
