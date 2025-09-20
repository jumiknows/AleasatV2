import unittest
from parameterized import parameterized

import numpy as np

from alea.sim.algorithms.guidance.location_pointing import compute_location_pointing_quaternion, compute_target_quaternion, calculate_rotation
from alea.sim.math_lib import Quaternion, normalize

# keep the same seed for consistent tests
rgen = np.random.default_rng(885112)

class LocationPointingTest(unittest.TestCase):
    @parameterized.expand([[rgen.uniform(0.0, 1.0, 3), rgen.uniform(0.0, 1.0, 3)] for _ in range(10)])
    def test_calc_rotation(self, v1: np.ndarray, v2: np.ndarray):
        """Test calculate rotation from v1 -> v2"""
        v1, v2 = normalize(v1), normalize(v2)
        q = calculate_rotation(v1, v2)
        rotmat = q.to_DCM()
        
        np.testing.assert_almost_equal(rotmat @ v1, v2)
        np.testing.assert_almost_equal(rotmat.T @ v2, v1)

    # create 10 random params
    @parameterized.expand([[rgen.uniform(0.0, 10e3, 3), rgen.uniform(0.0, 10e3, 3), rgen.uniform(0.0, 1.0, 3)] for _ in range(10)])
    def test_location_pointing(self, r1: np.ndarray, r2: np.ndarray, v: np.ndarray):
        """Test location pointing computation"""
        q = compute_location_pointing_quaternion(r1, r2, normalize(v))
        rotmat = q.to_DCM()
        
        np.testing.assert_almost_equal(normalize(r2-r1), rotmat @ normalize(v))

    @parameterized.expand([[rgen.uniform(0.0, 1.0, 3), rgen.uniform(0.0, 1.0, 3)] for _ in range(10)])
    def test_target_quaternion(self, v_target: np.ndarray, v_spacecraft: np.ndarray):
        """Test target quaternion computation"""
        v_target, v_spacecraft = normalize(np.array(v_target)), normalize(np.array(v_spacecraft))
        q = compute_target_quaternion(v_target, v_spacecraft)
        rotmat = q.to_DCM()
        
        np.testing.assert_almost_equal(rotmat @ v_spacecraft, v_target)
