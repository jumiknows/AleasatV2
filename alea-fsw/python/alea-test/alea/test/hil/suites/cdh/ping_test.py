import numpy as np

from alea.test import timeout
from alea.test.hil import hil_test

class PingTest(hil_test.HILTest):

    @timeout.timeout(5)
    def test_ping(self):
        resp = self.ttc.ping()
        self.assertTrue(resp.is_success)

    @timeout.timeout(5)
    def test_echo(self):
        number = 0xA113311A
        array = np.array([[1, 2, 3],
                          [4, 5, 6]])
        arrayf = np.array([[1.5, 2.5, 3.5],
                           [4.5, 5.5, 6.5]])
        message = "Hello ALEASAT"

        resp = self.ttc.send_obc_cmd("TEST_ECHO", number, array, arrayf, message)

        self.assertTrue(resp.is_success)
        self.assertEqual(number, resp.data["number"])
        np.testing.assert_array_equal(array, resp.data["array"])
        np.testing.assert_array_equal(arrayf, resp.data["arrayf"]) # Arrays should be exactly equal because no operations are performed
        self.assertEqual(message, resp.data["message"])

if __name__ == '__main__':
    hil_test.main(PingTest)
