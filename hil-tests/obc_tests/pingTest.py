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

    @timeout.timeout(5)
    def test_ping(self):
        resp = self.obc.ping()
        self.assertTrue(resp.is_success)

    @timeout.timeout(5)
    def test_echo(self):
        number = 0xA113311A
        message = "Hello ALEASAT"
        resp = self.obc.send_cmd("TEST_ECHO", number, message)
        self.assertTrue(resp.is_success)
        self.assertEqual(number, resp.data["number"])
        self.assertEqual(message, resp.data["message"])

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(PingTest)
