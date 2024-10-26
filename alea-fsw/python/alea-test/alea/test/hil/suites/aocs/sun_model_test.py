import unittest
from datetime import datetime, timedelta
import csv

import numpy as np

from alea.test import timeout
from alea.test.hil import hil_test
from alea.test.stk.stk_data_reader import STKDataReader

class SunModelTest(hil_test.HILTest):

    def setUp(self) -> None:
        self.skipTest("SunModel not functional yet")

    @timeout.timeout(40)
    def test_sun_model(self):
        time_stk = []
        x = []
        y = []
        z = []

        with STKDataReader.data_file_path("Satellite1_Sun_Vector_J2000_year.csv") as file_path:
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

            resp = self.ttc.send_obc_cmd("TEST_ADCS_SUN_MODEL", time)

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

if __name__ == '__main__':
    hil_test.main(SunModelTest)
