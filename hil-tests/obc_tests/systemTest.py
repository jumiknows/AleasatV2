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
    def test_get_time(self):
        timestamp = self.obc.get_time()
        print(timestamp)
        # Should be within the first 2 seconds
        self.assertLessEqual(timestamp.to_timestamp(), 2)

    @timeout.timeout(5)
    def test_set_time(self):
        target_time = datetime.datetime(2023, 7, 1, 11, 22, 33)
        resp = self.obc.set_time(target_time)
        self.assertEqual(resp.code, resp.Code.SUCCESS)

        obc_current_time = self.obc.get_time()
        self.assertEqual(target_time, obc_current_time.date_time)

    # TODO ALEA-425 Re-enable after new scheduler
    # @timeout.timeout(10)
    # def test_schedule_command(self):
    #     # Schedule it 5 seconds in the future
    #     self.obc.send_raw("0 ping 20-1-1 0:0:5 0")
    #     log = self.wait_for_keyword("Ping")
    #     expected_time = datetime.datetime.strptime('20-1-1 0:0:5', '%y-%m-%d %H:%M:%S')
    #     delta = (expected_time - log.date_time.date_time).total_seconds()
    #     self.assertLessEqual(delta, 0)

    @timeout.timeout(5)
    def test_get_rtos_tasks(self):
        resp = self.obc.send_cmd("RTOS_TASKS")
        self.assertEqual(resp.code, resp.Code.SUCCESS)
        time.sleep(1) # Give time for rest of message to be sent

    @timeout.timeout(5)
    def test_get_rtos_info(self):
        resp = self.obc.send_cmd("RTOS_INFO")
        self.assertEqual(resp.code, resp.Code.SUCCESS)
        self.wait_for_keyword("OK")

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(SystemTest)
