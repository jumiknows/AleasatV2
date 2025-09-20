from alea.test import timeout
from alea.test.hil import hil_test

class MramTest(hil_test.HILTest):
    # SUCCESS path: addr 0x0, len 64
    # Timeout is in seconds, it will also record how long this test takes
    @timeout.timeout(10)
    def test_mram_rw_64(self):
        resp = self.ttc.send_obc_cmd("TEST_MRAM_RW", 0x0, 64)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], True)

    # SUCCESS path: addr 0x0, len 0
    @timeout.timeout(10)
    def test_mram_rw_0(self):
        resp = self.ttc.send_obc_cmd("TEST_MRAM_RW", 0x0, 0)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], True)

    # SUCCESS path: addr 0x64, len 1024
    @timeout.timeout(10)
    def test_mram_rw_1024(self):
        resp = self.ttc.send_obc_cmd("TEST_MRAM_RW", 0x64, 1024)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], True)

    # FAIL path 0: invalid address 
    @timeout.timeout(10)
    def test_mram_rw_addr_bad_len_32(self):
        resp = self.ttc.send_obc_cmd("TEST_MRAM_RW", 0x8000, 32)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], False)

if __name__ == '__main__':
    hil_test.main(MramTest)
