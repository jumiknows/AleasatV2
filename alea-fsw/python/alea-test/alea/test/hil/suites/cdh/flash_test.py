from alea.test import timeout
from alea.test.hil import hil_test

class FlashTest(hil_test.HILTest):

    # SUCCESS path: addr 0x0, len 64
    # Timeout is in seconds, it will also record how long this test takes
    @timeout.timeout(10)
    def test_flash_rw_0(self):
        resp = self.ttc.send_obc_cmd("TEST_FLASH_RW", 0x0, 64)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], True)

    # SUCCESS path: adddr 0x40, len 64
    @timeout.timeout(10)
    def test_flash_rw_40(self):
        resp = self.ttc.send_obc_cmd("TEST_FLASH_RW", 0x40, 64)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], True)

    # SUCCESS path: adddr 0x80, len 64
    @timeout.timeout(10)
    def test_flash_rw_80(self):
        resp = self.ttc.send_obc_cmd("TEST_FLASH_RW", 0x80, 64)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], True)

    # SUCCESS path: adddr 1FFFFC0
    @timeout.timeout(10)
    def test_flash_rw_last_64(self):
        resp = self.ttc.send_obc_cmd("TEST_FLASH_RW", 0x1FFFFC0, 64)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], True)

    # FAIL path 0: len = 32
    #    Len =/= 64 bytes
    @timeout.timeout(10)
    def test_flash_rw_addr_bad_len_32(self):
        resp = self.ttc.send_obc_cmd("TEST_FLASH_RW", 0x0, 32)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], False)

    # FAIL path 1: len = 128
    #    Len =/= 64 bytes
    @timeout.timeout(10)
    def test_flash_rw_addr_bad_len_128(self):
        resp = self.ttc.send_obc_cmd("TEST_FLASH_RW", 0x0, 128)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], False)

    # FAIL path: addr = 02000000
    #    Addr too large
    @timeout.timeout(10)
    def test_flash_rw_addr_too_large(self):
        resp = self.ttc.send_obc_cmd("TEST_FLASH_RW", 0x02000000, 64)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], False)

    # FAIL path: addr = 0x1
    #    Addr unaligned
    @timeout.timeout(10)
    def test_flash_rw_addr_unaligned_1(self):
        resp = self.ttc.send_obc_cmd("TEST_FLASH_RW", 0x1, 64)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], False)

    # FAIL path: addr = 01FFFC01
    #    Addr unaligned
    @timeout.timeout(10)
    def test_flash_rw_addr_unaligned_1fffc01(self):
        resp = self.ttc.send_obc_cmd("TEST_FLASH_RW", 0x01FFFC01, 64)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], False)

    # FAIL path: addr 0x0, len 64
    #    Write commands ignored; comparison fails
    @timeout.timeout(10)
    def test_flash_rw_sleep(self):
        self.ttc.send_obc_cmd("FLASH_SLEEP", True)
        resp = self.ttc.send_obc_cmd("TEST_FLASH_RW", 0x0, 64)
        self.ttc.send_obc_cmd("FLASH_SLEEP", False)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], False)

    # SUCCESS path: addr 0x0, len 64
    #    Awake; commands accepted
    @timeout.timeout(10)
    def test_flash_rw_wake(self):
        self.ttc.send_obc_cmd("FLASH_SLEEP", True)
        self.ttc.send_obc_cmd("FLASH_SLEEP", False)
        resp = self.ttc.send_obc_cmd("TEST_FLASH_RW", 0x0, 64)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], True)

if __name__ == '__main__':
    hil_test.main(FlashTest)
