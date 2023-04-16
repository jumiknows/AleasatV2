# The following imports are required
from obc_test_internals import obc_test, timeout
import datetime
import time

"""
You want to inherit the Setup and Teardown of OBCTest so add it as the Parent Class.
Should you want to change the default SetUp or tearDown you can implement it here.
"""

class SystemTest(obc_test.OBCTest):

    @timeout.timeout(5)
    def test_fw_info(self):
        resp = self.obc.send_cmd("FW_INFO")
        self.assertTrue(resp.is_success)

    @timeout.timeout(5)
    def test_get_time(self):
        timestamp = self.obc.get_time()
        # Should be within the first 2 seconds
        self.assertLessEqual(timestamp.to_timestamp(), 2)

    @timeout.timeout(5)
    def test_set_time(self):
        target_time = datetime.datetime(2023, 7, 1, 11, 22, 33)
        resp = self.obc.set_time(target_time)
        self.assertTrue(resp.is_success)

        obc_current_time = self.obc.get_time()
        self.assertEqual(target_time, obc_current_time.date_time)

    @timeout.timeout(10)
    def test_schedule_command(self):
        now = self.obc.get_time()

        # Schedule a PING command for 5 seconds in the future
        sched_time = (now + 5)
        pending_resp = self.obc.send_cmd("PING", date_time=sched_time)
        self.assertTrue(pending_resp)
        self.assertTrue(pending_resp.sched_resp.is_success_sched)

        # Wait for the response to the command
        response = pending_resp.wait(6)

        self.assertTrue(response)
        self.assertTrue(response.is_success)
        self.assertEqual(response.data_header.exec_datetime, sched_time)

    @timeout.timeout(5)
    def test_get_rtos_tasks(self):
        resp = self.obc.send_cmd("RTOS_TASKS")
        self.assertTrue(resp.is_success)
        time.sleep(1) # Give time for rest of message to be sent

    @timeout.timeout(5)
    def test_get_rtos_info(self):
        resp = self.obc.send_cmd("RTOS_INFO")
        self.assertTrue(resp.is_success)
        self.wait_for_signal("LOG_OBC_TASK", "RTOS_STATUS_OK");

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(SystemTest)
