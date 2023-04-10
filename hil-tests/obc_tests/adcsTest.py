# The following imports are required
from obc_test_internals import obc_test, timeout

"""
You want to inherit the Setup and Teardown of OBCTest so add it as the Parent Class.
Should you want to change the default SetUp or tearDown you can implement it here.
"""
class ADCSTest(obc_test.OBCTest):

    @timeout.timeout(5)
    def test_dsplib(self):
        x = 123.45
        y = -678.9

        resp = self.obc.send_cmd("TEST_DSPLIB", x, y)

        self.assertTrue(resp.is_success)
        self.assertAlmostEqual(abs(x), resp.data["x_abs"], places=4)
        self.assertAlmostEqual(abs(y), resp.data["y_abs"], places=4)

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(ADCSTest)
