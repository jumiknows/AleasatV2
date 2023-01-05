"""
Test the read and write functions of the OBC flash driver
"""
# The following imports are required
from obc_test_internals import obc_test, timeout

"""
You want to inherit the Setup and Teardown of OBCTest so add it as the Parent Class.
Should you want to change the default SetUp or tearDown you can implement it here.
"""
class FlashTest(obc_test.OBCTest):

    # SUCCESS path: addr 0x0, len 64
    # Timeout is in seconds, it will also record how long this test takes
    @timeout.timeout(10)
    def test_flash_rw_0(self):
        resp = self.obc.send_cmd("TEST_FLASH_RW", 0x0, 64)
        self.assertEqual(resp.code, resp.Code.SUCCESS)
        self.assertEqual(resp["data_match"], True)

    # SUCCESS path: adddr 0x40, len 64
    @timeout.timeout(10)
    def test_flash_rw_40(self):
        resp = self.obc.send_cmd("TEST_FLASH_RW", 0x40, 64)
        self.assertEqual(resp.code, resp.Code.SUCCESS)
        self.assertEqual(resp["data_match"], True)

    # SUCCESS path: adddr 0x80, len 64
    @timeout.timeout(10)
    def test_flash_rw_80(self):
        resp = self.obc.send_cmd("TEST_FLASH_RW", 0x80, 64)
        self.assertEqual(resp.code, resp.Code.SUCCESS)
        self.assertEqual(resp["data_match"], True)

    # SUCCESS path: adddr 1FFFFC0
    @timeout.timeout(10)
    def test_flash_rw_last_64(self):
        resp = self.obc.send_cmd("TEST_FLASH_RW", 0x1FFFFC0, 64)
        self.assertEqual(resp.code, resp.Code.SUCCESS)
        self.assertEqual(resp["data_match"], True)

    # FAIL path 0: len = 32
    #    Len =/= 64 bytes
    @timeout.timeout(10)
    def test_flash_rw_addr_bad_len_32(self):
        resp = self.obc.send_cmd("TEST_FLASH_RW", 0x0, 32)
        self.assertEqual(resp.code, resp.Code.SUCCESS)
        self.assertEqual(resp["data_match"], False)

    # FAIL path 1: len = 128
    #    Len =/= 64 bytes
    @timeout.timeout(10)
    def test_flash_rw_addr_bad_len_128(self):
        resp = self.obc.send_cmd("TEST_FLASH_RW", 0x0, 128)
        self.assertEqual(resp.code, resp.Code.SUCCESS)
        self.assertEqual(resp["data_match"], False)

    # FAIL path: addr = 02000000
    #    Addr too large
    @timeout.timeout(10)
    def test_flash_rw_addr_too_large(self):
        resp = self.obc.send_cmd("TEST_FLASH_RW", 0x02000000, 64)
        self.assertEqual(resp.code, resp.Code.SUCCESS)
        self.assertEqual(resp["data_match"], False)

    # FAIL path: addr = 0x1
    #    Addr unaligned
    @timeout.timeout(10)
    def test_flash_rw_addr_unaligned_1(self):
        resp = self.obc.send_cmd("TEST_FLASH_RW", 0x1, 64)
        self.assertEqual(resp.code, resp.Code.SUCCESS)
        self.assertEqual(resp["data_match"], False)

    # FAIL path: addr = 01FFFC01
    #    Addr unaligned
    @timeout.timeout(10)
    def test_flash_rw_addr_unaligned_1fffc01(self):
        resp = self.obc.send_cmd("TEST_FLASH_RW", 0x01FFFC01, 64)
        self.assertEqual(resp.code, resp.Code.SUCCESS)
        self.assertEqual(resp["data_match"], False)

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(FlashTest)
