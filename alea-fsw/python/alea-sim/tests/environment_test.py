import unittest
from parameterized import parameterized

import datetime as dt

import numpy as np

from alea.sim.epa.magnetic_field_model import EarthMagneticFieldModel
from alea.sim.kernel.kernel import AleasimKernel

from pygeomag import GeoMag

from alea.sim.kernel.time_utils import toYearFraction

class MagneticEnvironmentTest(unittest.TestCase):
    """
    Mag test ref values: https://www.ngdc.noaa.gov/geomag/calculators/magcalc.shtml?useFullSite=true#igrfwmm
    """

    @parameterized.expand([
        [80.0, 0.0, 0.0, 2020.0, np.array([6570.4, -146.3, 54606.0])],
        [0.0, 120.0, 0.0, 2020.0, np.array([39624.3, 109.9, -10932.5])],
        [-80.0, 240.0, 100.0, 2022.5, np.array([5815.0, 14803.0, -49755.3])],
    ])
    def test_mag_model(self, lat, lon, alt, date, expected):
        #magnetic model test
        kernel = AleasimKernel(date=date) #init kernal at the day this test was created
        mag_model = EarthMagneticFieldModel(kernel)
            
        vec = mag_model.get_mag_vector_at_time(np.array([lat,lon,alt*1000]), date)
        
        #test to the precision of the dataset
        np.testing.assert_array_almost_equal(vec, expected, decimal=1)

    @parameterized.expand([
        [80.0, 0.0, 0.0, 2020.0, np.array([6570.4, -146.3, 54606.0])],
        [0.0, 120.0, 0.0, 2020.0, np.array([39624.3, 109.9, -10932.5])],
        [-80.0, 240.0, 100.0, 2022.5, np.array([5815.0, 14803.0, -49755.3])],
    ])
    def test_pygeomag_wmm(self, lat, lon, alt, time, expected):
        def result_to_numpy(r):
            return np.array([r.x, r.y, r.z])
            
        geo_mag = GeoMag()
        r = result_to_numpy(geo_mag.calculate(glat=lat, glon=lon, alt=alt, time=time))
        
        #test to the precision of the dataset
        np.testing.assert_array_almost_equal(r, expected, decimal=1)

if __name__ == '__main__':
    unittest.main()
