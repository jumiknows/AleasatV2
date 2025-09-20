import numpy as np

from alea.test import timeout
from alea.test.hil import hil_test

class HugeTxTest(hil_test.HILTest):

    @timeout.timeout(5)
    def test_ping(self):
        resp = self.ttc.ping()
        self.assertTrue(resp.is_success)

    @timeout.timeout(600)
    def test_huge_tx(self):
        data_len = 1024*50
        data = bytes([255] * data_len)

        resp = self.ttc.send_obc_cmd("TEST_LARGE_PAYLOAD_SPLIT", data, timeout=600)

        self.assertTrue(resp.is_success)

        received_sum = resp.data["sum"]

        self.assertEqual(255*data_len, received_sum)

if __name__ == '__main__':
    hil_test.main(HugeTxTest)
