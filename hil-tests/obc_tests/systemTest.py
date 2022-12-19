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
        self.obc.send("get_time")
        self.wait_for_keyword("20-1-1 0:0:")

    @timeout.timeout(5)
    def test_set_time(self):
        # Format is RTC Y M D H M S
        self.obc.send("set_time", "a", 21, 11, 17, 18, 1, 0)
        self.wait_for_keyword("Updated time on A")

        self.obc.send("get_time")
        self.wait_for_keyword("21-11-17 18:1:")

    @timeout.timeout(10)
    def test_schedule_command(self):
        # Schedule it 5 seconds in the future
        self.obc.send_raw("0 ping 20-1-1 0:0:5 0")
        log = self.wait_for_keyword("Ping")
        expected_time = datetime.datetime.strptime('20-1-1 0:0:5', '%y-%m-%d %H:%M:%S')
        delta = (expected_time - log.date_time.date_time).total_seconds()
        self.assertLessEqual(delta, 0)

    @timeout.timeout(5)
    def test_get_epoch(self):
        self.obc.send("get_epoch")
        self.wait_for_keyword("Epoch:")

    @timeout.timeout(5)
    def test_get_rtos_tasks(self):
        self.obc.send("rtos_tasks")
        self.wait_for_keyword("num_tasks")
        time.sleep(1) # Give time for rest of message to be sent

    @timeout.timeout(5)
    def test_get_rtos_info(self):
        self.obc.send("rtos_info")
        self.wait_for_keyword("OK")

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(SystemTest)
