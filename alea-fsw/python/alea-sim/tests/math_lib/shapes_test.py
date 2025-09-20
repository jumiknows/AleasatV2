import unittest
from parameterized import parameterized

import numpy as np

from alea.sim.math_lib.shapes import Spheroid

class SpheroidTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.spheroid = Spheroid(a=1.0, f=0.2)

    @parameterized.expand([
        [1.00, 0.20, 0.80],
        [3.00, 0.00, 3.00],
        [0.75, 0.70, 0.75*0.3],
    ])
    def test_axes(self, a, f, b):
        spheroid = Spheroid(a=a, f=f)
        self.assertEqual(spheroid.semi_major_axis, a)
        self.assertEqual(spheroid.flattening, f)
        self.assertAlmostEqual(spheroid.semi_minor_axis, b, places=8)

    @parameterized.expand([
        [0.0, 0.0, 0.0, False],
        [1.0, 0.0, 0.0, True],
        [1.1, 0.0, 0.0, False],
        [0.0, 1.0, 0.0, True],
        [0.0, 0.9, 0.0, False],
        [0.0, 0.0, 0.8, True],
        [0.0, 0.0, 0.9, False],
        [1/np.sqrt(2), 1/np.sqrt(2), 0.0, True],
        [1.0, 1.0, 0.0, False],
        [0.0, 1.0, 1.0, False],
        [1.0, 1.0, 1.0, False],
        [0.3, 0.4, np.sqrt(0.75)*0.8, True],
    ])
    def test_is_on_surface(self, x, y, z, expected):
        is_on_surface = self.spheroid.is_on_surface(x, y, z)
        self.assertEqual(is_on_surface, expected)

    @parameterized.expand([
        [0.0, 0.0],
        [45.0, 1/np.sqrt(2)],
        [90.0, 1.0],
        [120.0, np.sqrt(3)/2],
        [180.0, 0.0],
    ])
    def test_radius(self, theta_deg, expected):
        self.assertAlmostEqual(self.spheroid.radius(theta_deg), expected, places=8)

    def test_cartesian_surface_grid_coords(self):
        x, y, z = self.spheroid.cartesian_surface_grid(phi_step_deg=90, theta_step_deg=90)
        # Check x coordinates
        np.testing.assert_array_almost_equal(x, np.array([
            #   theta
            # 0   90   180    # phi
            [ 0 ,  1 ,  0  ], # 0
            [ 0 ,  0 ,  0  ], # 90
            [ 0 , -1 ,  0  ], # 180
            [ 0 ,  0 ,  0  ], # 270
            [ 0 ,  1 ,  0  ], # 360
        ]))
        # Check y coordinates
        np.testing.assert_array_almost_equal(y, np.array([
            #   theta
            # 0   90   180    # phi
            [ 0 ,  0 ,  0  ], # 0
            [ 0 ,  1 ,  0  ], # 90
            [ 0 ,  0 ,  0  ], # 180
            [ 0 , -1 ,  0  ], # 270
            [ 0 ,  0 ,  0  ], # 360
        ]))
        # Check z coordinates
        np.testing.assert_array_almost_equal(z, np.array([
            #   theta
            #  0   90    180    # phi
            [ 0.8 , 0 , -0.8 ], # 0
            [ 0.8 , 0 , -0.8 ], # 90
            [ 0.8 , 0 , -0.8 ], # 180
            [ 0.8 , 0 , -0.8 ], # 270
            [ 0.8 , 0 , -0.8 ], # 360
        ]))

    @parameterized.expand([
        # phi, theta, (n_phi, n_theta)
        [  90,   90,  (5, 3)],
        [  90,   30,  (5, 7)],
        [  45,   90,  (9, 3)],
    ])
    def test_cartesian_surface_grid_shape(self, phi_step_deg, theta_step_deg, expected_shape):
        x, y, z = self.spheroid.cartesian_surface_grid(phi_step_deg=phi_step_deg, theta_step_deg=theta_step_deg)
        self.assertEqual(x.shape, expected_shape)
        self.assertEqual(y.shape, expected_shape)
        self.assertEqual(z.shape, expected_shape)

    @parameterized.expand([
        [  0,   0, ( 0.0,  0.0,  0.8)],
        [ 90,   0, ( 0.0,  0.0,  0.8)],
        [  0,  90, ( 1.0,  0.0,  0.0)],
        [ 90,  90, ( 0.0,  1.0,  0.0)],
        [180,  90, (-1.0,  0.0,  0.0)],
        [270,  90, ( 0.0, -1.0,  0.0)],
        [  0, 180, ( 0.0,  0.0, -0.8)],
        [270, 180, ( 0.0,  0.0, -0.8)],
    ])
    def test_cartesian_surface_point_coords(self, phi_deg, theta_deg, expected_point):
        point = self.spheroid.cartesian_surface_point(phi_deg, theta_deg)
        np.testing.assert_array_almost_equal(np.array(point), np.array(expected_point))

    def test_cartesian_surface_point_on_surface(self):
        for phi_deg in np.linspace(0, 180, 20):
            for theta_deg in np.linspace(0, 360, 20):
                with self.subTest(phi_deg=phi_deg, theta_deg=theta_deg):
                    point = self.spheroid.cartesian_surface_point(phi_deg, theta_deg)
                    self.assertTrue(self.spheroid.is_on_surface(*point))

    @parameterized.expand([
        #               origin       ,               direction     , distance ,              intersection
        # From +x axis
        [np.array([ 2.0,  0.0,  0.0]), np.array([-1.0,  0.0,  0.0]),    1.0   , np.array([ 1.0,  0.0,  0.0])],
        [np.array([ 2.0,  0.0,  0.0]), np.array([ 1.0,  0.0,  0.0])],
        # From -y axis
        [np.array([ 0.0, -1.5,  0.0]), np.array([ 0.0,  1.0,  0.0]),    0.5   , np.array([ 0.0, -1.0,  0.0])],
        [np.array([ 0.0, -1.5,  0.0]), np.array([ 0.0, -1.0,  0.0])],
        # From +z axis
        [np.array([ 0.0,  0.0,  2.5]), np.array([ 0.0,  0.0, -1.0]),    1.7   , np.array([ 0.0,  0.0,  0.8])],
        [np.array([ 0.0,  0.0, -2.5]), np.array([ 0.0,  0.0, -1.0])],
    ])
    def test_surface_intersection_point_coords(self, origin, direction, exp_distance=None, exp_intersection=None):
        distance, intersection = self.spheroid.surface_intersection_point(origin, direction)

        if exp_distance is None:
            self.assertTrue(np.isnan(distance))
        else:
            self.assertAlmostEqual(distance, exp_distance)

        if exp_intersection is None:
            self.assertTrue(np.all(np.isnan(intersection)))
        else:
            np.testing.assert_array_almost_equal(intersection, exp_intersection)

if __name__ == "__main__":
    unittest.main(SpheroidTest)
