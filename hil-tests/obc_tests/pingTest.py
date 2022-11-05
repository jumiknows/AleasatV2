"""
This file shows how you can create an alea unitest
"""
# The following imports are required
from obc_test_internals import obc_test, timeout

"""
You want to inherit the Setup and Teardown of OBCTest so add it as the Parent Class.
Should you want to change the default SetUp or tearDown you can implement it here.
"""
class PingTest(obc_test.OBCTest):

    # Timeout is in seconds, it will also record how long this test takes.
    @timeout.timeout(5)
    def test_ping(self):
        self.obc.ping()
        while 1:
            msg = self.obc.recv()
            if "Ping" in msg.payload:
                return

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(PingTest)
