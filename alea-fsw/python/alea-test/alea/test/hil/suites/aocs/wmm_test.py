from datetime import date

import numpy as np
from parameterized import parameterized
import ahrs

from alea.test import timeout
from alea.test.hil import hil_test

class WmmTest(hil_test.HILTest):
    # test assertion tolerance
    PERCENTAGE_TOLERANCE = 1
    TOLERANCE = 1e-3

    # test params
    d1=date(2021,1,1)
    d2=date(2024,7,2)
    test_values = [
        # date, alt, lat, lon
        [d1, 1.0, 80.0, 0.0],
        [d1, 1.0, 0.0, 120.0],
        [d1, 1.0, -80.0, 140.0],
        [d1, 500.0, 80.0, 0.0],
        [d1, 328.0, 0.0, 120.0],
        [d1, 328.0, -80.0, 160.0],
        [d1, 1.0, -80.0, -170.0],
        [d1, 100.0, -80.0, -170.0],
        [d1, -8.0, -80.0, -170.0],
        [d1, 0.0, 0.0, 0.0],
        [d1, 0.0, 90.0, 180.0],
        [d1, 100.0, 80.0, 180.0],
        [d1, 450.0, 0.0, 0.0],
        [d1, 450.0, 90.0, 180.0],
        [d1, 450.0, -80.0, -170.0],
        [d1, 450.0, 90.0, -180.0],
        [d2, 1.0, 80.0, 0.0],
        [d2, 1.0, 0.0, 120.0],
        [d2, 1.0, -80.0, 140.0],
        [d2, 500.0, 80.0, 0.0],
        [d2, 328.0, 0.0, 120.0],
        [d2, 328.0, -80.0, 160.0],
        [d2, 1.0, -80.0, -170.0],
        [d2, 100.0, -80.0, -170.0],
        [d2, -8.0, -80.0, -170.0],
        [d2, 0.0, 0.0, 0.0],
        [d2, 0.0, 90.0, 180.0],
        [d2, 100.0, 80.0, 180.0],
        [d2, 450.0, 0.0, 0.0],
        [d2, 450.0, 90.0, 180.0],
        [d2, 450.0, -80.0, -170.0],
        [d2, 450.0, 90.0, -180.0],
    ]
    ahrs_wmm = ahrs.utils.WMM()

    @timeout.timeout(10)
    def test_wmm_init(self):
        self.ttc.send_obc_cmd("WMM_INIT")
        resp = self.ttc.send_obc_cmd("WMM_IS_INIT")
        self.assertTrue(resp.is_success)
        self.assertTrue(resp.data["is_init"])

    @timeout.timeout(5)
    def test_wmm_no_init(self):
        resp = self.ttc.send_obc_cmd("WMM_IS_INIT")
        self.assertTrue(resp.is_success)
        self.assertFalse(resp.data["is_init"])

    @timeout.timeout(5)
    def test_wmm_no_init_call_mag_ref(self):
        resp = self.ttc.send_obc_cmd("WMM_GET_MAG_REF", 1, 1, 1, self.d1.year-2000, self.d1.month, self.d1.day)
        self.assertFalse(resp.is_success)

    @timeout.timeout(10)
    def test_wmm_init_call_mag_ref(self):
        self.ttc.send_obc_cmd("WMM_INIT")
        resp = self.ttc.send_obc_cmd("WMM_GET_MAG_REF", 1, 1, 1, self.d1.year-2000, self.d1.month, self.d1.day)
        self.assertTrue(resp.is_success)

    @parameterized.expand(test_values)
    @timeout.timeout(10)
    def test_mag_ref_similarity(self, date, alt, lat, lon):
        """Test get_mag_ref command against ahrs python library
        https://ahrs.readthedocs.io/en/latest/wmm.html.
        Test component similarity and fail test if any component 
        has a percentage difference larger than specified below."""
        if not (self.ttc.send_obc_cmd("WMM_IS_INIT")).data["is_init"]:
            self.ttc.send_obc_cmd("WMM_INIT")
        self.ahrs_wmm.magnetic_field(longitude=lon, latitude=lat, height=alt, date=date)
        ahrs_mag_ref = np.array([self.ahrs_wmm.X, self.ahrs_wmm.Y, self.ahrs_wmm.Z])
        resp = self.ttc.send_obc_cmd("WMM_GET_MAG_REF", lon, lat, alt, date.year-2000, date.month, date.day)
        self.assertTrue(resp.is_success)
        self.assertEqualRelativeVector(ahrs_mag_ref, np.array(resp.data["mag_ref"]), self.TOLERANCE)

if __name__ == '__main__':
    hil_test.main(WmmTest)
