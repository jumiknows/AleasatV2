from alea.test import timeout
from alea.test.hil import hil_test

class FilesystemTest(hil_test.HILTest):

    # Timeout is in seconds, it will also record how long this test takes
    @timeout.timeout(10)
    def test_fs_cmd(self):
        resp = self.ttc.send_obc_cmd("TEST_FILESYSTEM")
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["fs_err"], 0)

if __name__ == '__main__':
    hil_test.main(FilesystemTest)
