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
        while 1:
            msg = self.obc.recv()
            # Allow for a few seconds to have passed
            if "20-1-1 0:0:" in msg.payload:
                return

    @timeout.timeout(5)
    def test_set_time(self):
        # Format is RTC Y M D H M S
        self.obc.send("set_time", "a", 21, 11, 17, 18, 1, 0)
        while 1:
            msg = self.obc.recv()
            if "Updated time on A" in msg.payload:
                break

        self.obc.send("get_time")
        while 1:
            msg = self.obc.recv()
            if "21-11-17 18:1:" in msg.payload:
                # they can take at most 1 min
                return

    @timeout.timeout(10)
    def test_schedule_command(self):
        # Schedule it 5 seconds in the future
        self.obc.send_raw("0 ping 20-1-1 0:0:5 0")
        while 1:
            msg = self.obc.recv()
            if "Ping" in msg.payload:
                expected_time = datetime.datetime.strptime('20-1-1 0:0:5', '%y-%m-%d %H:%M:%S')
                delta = (expected_time - msg.timestamp).total_seconds()
                self.assertLessEqual(delta, 0)
                return

    @timeout.timeout(5)
    def test_get_epoch(self):
        self.obc.send("get_epoch")
        while 1:
            msg = self.obc.recv()
            if 'Epoch:' in msg.payload:
                return

    @timeout.timeout(5)
    def test_get_rtos_tasks(self):
        self.obc.send("rtos_tasks")
        while 1:
            msg = self.obc.recv()
            # testing for this is quite difficult right now check for num_tasks,
            # in the future we might want a test that turns on a task and then
            # we check that the counter increased
            if "num_tasks" in msg.payload:
                time.sleep(1) # Give time for rest of message to be sent
                return

    @timeout.timeout(5)
    def test_get_rtos_info(self):
        self.obc.send("rtos_info")
        while 1:
            msg = self.obc.recv()
            # To test this we might want to spin up a bunch of tasks and then
            # get too many tasks or something
            if "OK" in msg.payload:
                return

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(SystemTest)
