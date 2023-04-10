"""
Test the functionality of the OBC filesystem
"""
# The following imports are required
from obc_test_internals import obc_test, timeout

passKeyword = "PASS"
failKeyword = "FAIL"

"""
You want to inherit the Setup and Teardown of OBCTest so add it as the Parent Class.
Should you want to change the default SetUp or tearDown you can implement it here.
"""
class FilesystemTest(obc_test.OBCTest):

    # Timeout is in seconds, it will also record how long this test takes
    @timeout.timeout(10)
    def test_fs_cmd(self):
        resp = self.obc.send_cmd("TEST_FILESYSTEM")
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["fs_err"], 0)

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(FilesystemTest)
