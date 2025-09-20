import unittest
from parameterized import parameterized

from dataclasses import dataclass
from math import radians

import numpy as np

from skyfield.api import wgs84
from skyfield.positionlib import Geocentric
from skyfield.toposlib import GeographicPosition

from alea.sim.configuration import Configurable
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel, OrbitDynamicsComputeFunc, OrbitDynamicsConfig

from pygeomag import GeoMag

@dataclass
class MockOrbitDynamicsComputeFunc(OrbitDynamicsComputeFunc):
    """Subclass of OrbitDynamicsComputeFunc that always returns the same fixed
    geographic position (latitude, longitude, altitude).
    """

    force_geo_position: list[float] = None # lat (rad), lon (rad), alt (m)

    def __getstate__(self) -> tuple:
        return (*super().__getstate__(), self.force_geo_position)

    def __setstate__(self, state: tuple):
        """Restore state from the unpickled state values."""
        self.force_geo_position = state[-1]
        super().__setstate__(state[:-1])

    def calc_geo_positions(self, sat_states: Geocentric) -> tuple[GeographicPosition, np.ndarray]:
        geo_position = wgs84.latlon(*self.force_geo_position)
        return (geo_position, np.array([self.force_geo_position]))

class EarthMagneticFieldTest(unittest.TestCase):
    """
    Mag test ref values: https://www.ngdc.noaa.gov/geomag/calculators/magcalc.shtml?useFullSite=true#igrfwmm
    """

    @parameterized.expand([
        [80.0, 0.0, 0.0, 2020.0, np.array([6570.4, -146.3, 54606.0])],
        [0.0, 120.0, 0.0, 2020.0, np.array([39624.3, 109.9, -10932.5])],
        [-80.0, 240.0, 100.0, 2022.5, np.array([5815.0, 14803.0, -49755.3])],
    ])
    def test_mag_model(self, lat, lon, alt, date, expected):
        kernel = AleasimKernel(date=date) #init kernal at the day this test was created

        # Override the orbit dynamics to return a specific geographic position
        odyn_cfg = Configurable("orbit_dynamics", OrbitDynamicsConfig)
        odyn_compute_func = MockOrbitDynamicsComputeFunc(kernel.timescale, tle=odyn_cfg.cfg.tle,
                                                         force_geo_position=[
                                                             radians(lat),
                                                             radians(lon),
                                                             alt * 1000 # km to m
                                                         ])

        odyn_model = OrbitDynamicsModel(kernel, compute_func=odyn_compute_func)
        mag_model = EarthMagneticFieldModel(kernel)

        kernel.add_model(odyn_model)
        kernel.add_model(mag_model)

        kernel.step()

        vec = mag_model.mag_field_vector_ned

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
