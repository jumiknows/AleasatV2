# The following imports are required
from obc_test_internals import OBCTestClass, timeout
import argparse, unittest, time, datetime

"""
You want to inherit the Setup and Teardown of OBCTest so add it as the Parent Class.
Should you want to change the default SetUp or tearDown you can implement it here.
"""


class SystemTest(OBCTestClass.OBCTest):

    # Timeout is in seconds, it will also record how long this test takes.
    @timeout.timeout(10)
    def test_get_time(self):
        self.obc.send_q.put("0 get_time * *")
        while 1:
            msg = self.obc.recv_q.get(block=True)
            if "-" and ":" in msg.print():
                return

    @timeout.timeout(60)
    def test_set_time(self):
        # Format is RTC Y M D H M S
        self.obc.send_q.put("0 set_time * * a 21 11 17 18 1 0")
        while 1:
            msg = self.obc.recv_q.get(block=True)
            if "Updated time on A" in msg.print():
                break

        self.obc.send_q.put("0 get_time * *")
        while 1:
            msg = self.obc.recv_q.get(block=True)
            if "21-11-17 18:1" in msg.print():
                # they can take at most 1 min
                return

    @timeout.timeout(140)
    def test_schedule_command(self):
        # Schedule it 2 minutes in the future
        self.obc.send_q.put("0 ping 20-1-1 0:2:0 0")
        while 1:
            msg = self.obc.recv_q.get(block=True)
            msg_str, timestamp = msg.print(timestamp=True)
            if "Ping" in msg_str:
                expected_time = datetime.datetime.strptime('20-1-1 0:2:0', '%y-%m-%d %H:%M:%S')
                assert (expected_time - timestamp) == datetime.timedelta(0), print("Delta", expected_time - timestamp)
                return

    @timeout.timeout(10)
    def test_get_epoch(self):
        self.obc.send_q.put("0 get_epoch * *")
        while 1:
            msg = self.obc.recv_q.get(block=True)
            if 'Epoch:' in msg.print():
                return

    @timeout.timeout(10)
    def test_get_rtos_tasks(self):
        self.obc.send_q.put("0 rtos_tasks * *")
        while 1:
            msg = self.obc.recv_q.get(block=True)
            # testing for this is quite difficult right now check for num_tasks,
            # in the future we might want a test that turns on a task and then
            # we check that the counter increased
            if "num_tasks" in msg.print():
                return

    @timeout.timeout(10)
    def test_get_rtos_info(self):
        self.obc.send_q.put("0 rtos_info * *")
        while 1:
            msg = self.obc.recv_q.get(block=True)
            # To test this we might want to spin up a bunch of tasks and then
            # get too many tasks or something
            if "OK" in msg.print():
                return

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--listports", default="False")
    parser.add_argument("--port", default="COM8")
    args = vars(parser.parse_args())

    if args["listports"] == "True":
        print(OBCTestClass.get_serial_ports())
        exit(0)

    SystemTest.PORT = args["port"]
    try:
        unittest.main()
    except Exception as e:
        print("Test timed out!")
