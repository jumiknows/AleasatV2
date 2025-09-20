import unittest
from parameterized import parameterized

import numpy as np

from alea.sim.math_lib import coord_utils

class CoordUtilsTest(unittest.TestCase):
    @parameterized.expand([
        [1.0, 0.0, (1.0, 0.0)],
        [0.5, np.pi/2, (0.0, 0.5)],
        [2.0, -np.pi/4, (2.0/np.sqrt(2), -2.0/np.sqrt(2))],
    ])
    def test_polar_to_cartesian(self, r, phi, expected):
        cartesian = coord_utils.polar_to_cartesian(r, phi)
        np.testing.assert_array_almost_equal(np.array(cartesian), np.array(expected))

    @parameterized.expand([
        [1.0, 0.0, (1.0, 0.0)],
        [0.0, 0.5, (0.5, np.pi/2)],
        [2.0/np.sqrt(2), -2.0/np.sqrt(2), (2.0, -np.pi/4)],
    ])
    def test_cartesian_to_polar(self, x, y, expected):
        polar = coord_utils.cartesian_to_polar(x, y)
        np.testing.assert_array_almost_equal(np.array(polar), np.array(expected))

    def test_polar_to_cartesian_invertible(self):
        for r in np.linspace(0.1, 2.0, 10):
            for phi in np.linspace(-np.pi, np.pi, 10):
                with self.subTest(r=r, phi=phi):
                    x, y = coord_utils.polar_to_cartesian(r, phi)
                    r_2, phi_2 = coord_utils.cartesian_to_polar(x, y)
                    self.assertAlmostEqual(r, r_2, places=8)
                    self.assertAlmostEqual(phi, phi_2, places=8)

    def test_cartesian_to_polar_invertible(self):
        for x in np.linspace(-2.0, 2.0, 10):
            for y in np.linspace(-2.0, 2.0, 10):
                with self.subTest(x=x, y=y):
                    r, phi = coord_utils.cartesian_to_polar(x, y)
                    x_2, y_2 = coord_utils.polar_to_cartesian(r, phi)
                    self.assertAlmostEqual(x, x_2, places=8)
                    self.assertAlmostEqual(y, y_2, places=8)

    @parameterized.expand([
        [1.0, 0.0, 0.0, (0.0, 0.0, 1.0)],
        [2.0, 0.0, np.pi/2, (2.0, 0.0, 0.0)],
        [0.6, np.pi/4, np.pi/2, (0.6/np.sqrt(2), 0.6/np.sqrt(2), 0.0)],
    ])
    def test_spherical_to_cartesian(self, r, phi, theta, expected):
        cartesian = coord_utils.spherical_to_cartesian(r, phi, theta)
        np.testing.assert_array_almost_equal(np.array(cartesian), np.array(expected))

    @parameterized.expand([
        [0.0, 0.0, 1.0, (1.0, 0.0, 0.0)],
        [2.0, 0.0, 0.0, (2.0, 0.0, np.pi/2)],
        [0.6/np.sqrt(2), 0.6/np.sqrt(2), 0.0, (0.6, np.pi/4, np.pi/2)],
    ])
    def test_cartesian_to_spherical(self, x, y, z, expected):
        spherical = coord_utils.cartesian_to_spherical(x, y, z)
        np.testing.assert_array_almost_equal(np.array(spherical), np.array(expected))

    def test_spherical_to_cartesian_invertible(self):
        for r in np.linspace(0.1, 2.0, 5):
            for phi in np.linspace(-np.pi, np.pi, 5):
                for theta in np.linspace(0, np.pi, 5):
                    with self.subTest(r=r, phi=phi, theta=theta):
                        x, y, z = coord_utils.spherical_to_cartesian(r, phi, theta)
                        r_2, phi_2, theta_2 = coord_utils.cartesian_to_spherical(x, y, z)
                        self.assertAlmostEqual(r, r_2, places=8)
                        if (theta == 0) or (theta == np.pi):
                            # Expected angle is actually phi when theta is polar
                            phi = 0
                        self.assertAlmostEqual(phi, phi_2, places=8)
                        self.assertAlmostEqual(theta, theta_2, places=8)

    def test_cartesian_to_spherical_invertible(self):
        for x in np.linspace(-2.0, 2.0, 5):
            for y in np.linspace(-2.0, 2.0, 5):
                for z in np.linspace(-2.0, 2.0, 5):
                    with self.subTest(x=x, y=y, z=z):
                        r, phi, theta = coord_utils.cartesian_to_spherical(x, y, z)
                        x_2, y_2, z_2 = coord_utils.spherical_to_cartesian(r, phi, theta)
                        self.assertAlmostEqual(x, x_2, places=8)
                        self.assertAlmostEqual(y, y_2, places=8)
                        self.assertAlmostEqual(z, z_2, places=8)
