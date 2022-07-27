"""
Test the functionality of the OBC filesystem
"""
# The following imports are required
from obc_test_internals import OBCTestClass, timeout
import argparse, unittest

passKeyword = "PASS"
failKeyword = "FAIL"

"""
You want to inherit the Setup and Teardown of OBCTest so add it as the Parent Class.
Should you want to change the default SetUp or tearDown you can implement it here.
"""
class FilesystemTest(OBCTestClass.OBCTest):

    # Timeout is in seconds, it will also record how long this test takes
    @timeout.timeout(10)
    def test_fs_cmd(self):
        self.obc.send_q.put("0 test_fs * *")
        while 1:
            msg = self.obc.recv_q.get(block=True)
            if passKeyword in msg.print():
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

    FilesystemTest.PORT = args["port"]
    try:
        unittest.main()
    except Exception as e:
        print("Test timed out!")
