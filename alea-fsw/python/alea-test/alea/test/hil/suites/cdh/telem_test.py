from alea.test import timeout
from alea.test.hil import hil_test
import time

class TelemTest(hil_test.HILTest):

    # Timeout is in seconds, it will also record how long this test takes
    @timeout.timeout(70)
    def test_telem_glv_cmd(self):
        # Erase the flash chip by calling TEST_FLASH_RW so we don't pick up old signals
        resp = self.ttc.send_obc_cmd("TEST_FLASH_RW", 0x0, 64)
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["data_match"], True)

        # Upon startup, the telem system should already be functional
        # We wait 60 seconds for the TEST_GET_EPOCH unit to activate once
        time.sleep(60)
        resp = self.ttc.send_obc_cmd("TEST_TELEM_GLV")
        self.assertTrue(resp.is_success)
        self.assertEqual(resp.data["epoch"], 60)

if __name__ == '__main__':
    hil_test.main(TelemTest)
