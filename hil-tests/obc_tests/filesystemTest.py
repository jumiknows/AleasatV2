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
        self.obc.send("test_fs")
        self.wait_for_keyword(passKeyword)

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(FilesystemTest)
