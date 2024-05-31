# The following imports are required
from obc_test_internals import obc_test, timeout
import numpy as np
from astropy.time import Time
from astropy.coordinates import get_sun
from astropy.coordinates.builtin_frames.utils import get_jd12
from astropy import units as u
from astropy.constants import c
import erfa
from astropy.coordinates import CartesianRepresentation, SphericalRepresentation, cartesian_to_spherical
import csv
from datetime import datetime, timedelta
import pathlib

"""
You want to inherit the Setup and Teardown of OBCTest so add it as the Parent Class.
Should you want to change the default SetUp or tearDown you can implement it here.
"""
ADCS_TEST_DIR = pathlib.Path(__file__).resolve().parent / "adcs_test_internals"

class SunModelTest(obc_test.OBCTest):

    @timeout.timeout(40)
    def test_sun_model(self):
        file_path = ADCS_TEST_DIR / "Satellite1_Sun_Vector_J2000_year.csv"
        time_stk = []
        x = []
        y = []
        z = []
        with open(file_path, 'r') as file:
            reader = csv.reader(file)
            next(reader)  # Skip header row

            for row in reader:
                if len(row) >= 4:
                    time_stk.append(row[0])
                    x.append(float(row[1]))
                    y.append(float(row[2]))
                    z.append(float(row[3]))

        start_time = datetime(2023, 6, 22, 19, 0, 0)
        max_delta_percent = 0
        for i in range(len(x)):
            time = [start_time.year,
                    start_time.month,
                    start_time.day,
                    start_time.hour,
                    start_time.minute,
                    start_time.second]

            resp = self.obc.send_cmd("TEST_ADCS_SUN_MODEL", time)

            self.assertTrue(resp.is_success)
            sun_pos = resp.data["pos"]
            jul_date = resp.data["jul"]
            sun_pos /= 149597870.7e3        # convert from km to AU
            sun_pos_mine = sun_pos
            sun_pos_stk = [x[i], y[i], z[i]]
            delta = np.linalg.norm(sun_pos_stk - sun_pos_mine)
            delta_percent = delta / np.linalg.norm(sun_pos_stk)
            
            if delta_percent > max_delta_percent:
                max_delta_percent = delta_percent
            
            self.assertEqualRelativeVector(sun_pos_mine, sun_pos_stk, 0.04)
            start_time += timedelta(days=1)
        
        print('\nMax delta is %f%%\n' % (max_delta_percent * 100))

"""
This section is required if you want to run these tests independently.
"""
if __name__ == '__main__':
    obc_test.main(SunModelTest)
