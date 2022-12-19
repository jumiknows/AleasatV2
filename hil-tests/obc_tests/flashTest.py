"""
Test the read and write functions of the OBC flash driver
"""
# The following imports are required
from obc_test_internals import obc_test, timeout

passKeyword = "PASS"
failKeyword = "FAIL"

"""
You want to inherit the Setup and Teardown of OBCTest so add it as the Parent Class.
Should you want to change the default SetUp or tearDown you can implement it here.
"""
class FlashTest(obc_test.OBCTest):

    # SUCCESS path: addr 0x0, len 64
    # Timeout is in seconds, it will also record how long this test takes
    @timeout.timeout(10)
    def test_flash_rw_0(self):
        self.obc.send("test_flash_rw", 0, 64)
        self.wait_for_keyword(passKeyword, failKeyword)

    # SUCCESS path: adddr 0x40, len 64
    @timeout.timeout(10)
    def test_flash_rw_40(self):
        self.obc.send("test_flash_rw", 40, 64)
        self.wait_for_keyword(passKeyword, failKeyword)

    # SUCCESS path: adddr 0x80, len 64
    @timeout.timeout(10)
    def test_flash_rw_80(self):
        self.obc.send("test_flash_rw", 80, 64)
        self.wait_for_keyword(passKeyword, failKeyword)

    # SUCCESS path: adddr 1FFFFC0
    @timeout.timeout(10)
    def test_flash_rw_last_64(self):
        self.obc.send("test_flash_rw", "1FFFFC0", 64)
        self.wait_for_keyword(passKeyword, failKeyword)

    # FAIL path 0: len = 32
    #    Len =/= 64 bytes
    @timeout.timeout(10)
    def test_flash_rw_addr_bad_len_32(self):
        self.obc.send("test_flash_rw", 0, 32)
        self.wait_for_keyword(failKeyword, passKeyword)

    # FAIL path 1: len = 128
    #    Len =/= 64 bytes
    @timeout.timeout(10)
    def test_flash_rw_addr_bad_len_128(self):
        self.obc.send("test_flash_rw", 0, 128)
        self.wait_for_keyword(failKeyword, passKeyword)

    # FAIL path: addr = 02000000
    #    Addr too large
    @timeout.timeout(10)
    def test_flash_rw_addr_too_large(self):
        self.obc.send("test_flash_rw", "02000000", 64)
        self.wait_for_keyword(failKeyword, passKeyword)

    # FAIL path: addr = 0x1
    #    Addr unaligned
    @timeout.timeout(10)
    def test_flash_rw_addr_unaligned_1(self):
        self.obc.send("test_flash_rw", 1, 64)
        self.wait_for_keyword(failKeyword, passKeyword)

    # FAIL path: addr = 01FFFC01
    #    Addr unaligned
    @timeout.timeout(10)
    def test_flash_rw_addr_unaligned_1fffc01(self):
        self.obc.send("test_flash_rw", "01FFFC01", 64)
        self.wait_for_keyword(failKeyword, passKeyword)

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(FlashTest)
