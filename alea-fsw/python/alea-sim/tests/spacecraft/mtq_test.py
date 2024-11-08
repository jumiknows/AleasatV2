import unittest
from parameterized import parameterized

import numpy as np

from alea.sim.kernel.kernel import AleasimKernel

from alea.sim.spacecraft.actuators.mtq_model import *

class MagnetorquerTest(unittest.TestCase):

    def test_mag_model_postive(self):
        """Basic test of generating dipole moment by setting positive duty cycle"""
        kernel = AleasimKernel(dt=0.01)
        normal_vec = np.array([2.5, 0.0, 0.0])     # Needs to be normalized to have a size of 1
        mtq = MagnetorquerModel('mtq', kernel, normal_vec=normal_vec)
        kernel.add_model(mtq, True)
        
        self.assertEqual(mtq.power_usage, 0.0)
        self.assertEqual(mtq.current, 0.0)
        
        mtq.power_on()

        mtq.set_duty_cycle(50)
        dipole_moment = mtq.get_moment()
        expected_dipole_moment = np.array([0.0876, 0.0, 0.0])
        np.testing.assert_almost_equal(dipole_moment, expected_dipole_moment, 3)

    def test_mag_model_postive_loop(self):
        """Generating dipole moment by setting positive duty cycle and running the simulation for a while. The magnetic moment produced is not consistant if test_mag_model_positive did not fail and this test failed"""
        kernel = AleasimKernel(dt=0.01)
        normal_vec = np.array([2.5, 0.0, 0.0])     # Needs to be normalized to have a size of 1
        mtq = MagnetorquerModel('mtq', kernel, normal_vec=normal_vec)
        kernel.add_model(mtq, True)
    
        self.assertEqual(mtq.power_usage, 0.0)
        self.assertEqual(mtq.current, 0.0)
    
        mtq.power_on()

        for _ in range(50):
            mtq.set_duty_cycle(50)
            dipole_moment = mtq.get_moment()
            expected_dipole_moment = np.array([0.0876, 0.0, 0.0])
            np.testing.assert_almost_equal(dipole_moment, expected_dipole_moment, 3)
            kernel.step()

    def test_mag_model_negative(self):
        """Basic test of generating dipole moment by setting negative duty cycle"""
        kernel = AleasimKernel(dt=0.01)
        normal_vec = np.array([2.5, 0.0, 0.0])     # Needs to be normalized to have a size of 1
        mtq = MagnetorquerModel('mtq', kernel, normal_vec=normal_vec)
        kernel.add_model(mtq, True)

        mtq.power_on()

        mtq.set_duty_cycle(-50)
        dipole_moment = mtq.get_moment()
        expected_dipole_moment = np.array([-0.0876, 0.0, 0.0])
        np.testing.assert_almost_equal(dipole_moment, expected_dipole_moment, 3)

    def test_mag_model_negative_loop(self):
        """Generating dipole moment by setting negative duty cycle and running the simulation for a while. The magnetic moment produced is not consistant if test_mag_model_negative did not fail and this test failed"""
        kernel = AleasimKernel(dt=0.01)
        normal_vec = np.array([2.5, 0.0, 0.0])     # Needs to be normalized to have a size of 1
        mtq = MagnetorquerModel('mtq', kernel, normal_vec=normal_vec)
        kernel.add_model(mtq, True)

        mtq.power_on()

        for _ in range(50):
            mtq.set_duty_cycle(-50)
            dipole_moment = mtq.get_moment()
            expected_dipole_moment = np.array([-0.0876, 0.0, 0.0])
            np.testing.assert_almost_equal(dipole_moment, expected_dipole_moment, 3)
            kernel.step()

    def test_mag_model_dutycycle_upper_bound(self):
        """Saturation test of generating positive dipole moment"""
        kernel = AleasimKernel(dt=0.01)
        normal_vec = np.array([0.0, 0.0, 1.0])
        mtq = MagnetorquerModel('mtq', kernel, saturation_moment=0.2, normal_vec=normal_vec)
        kernel.add_model(mtq, True)
        mtq.power_on()

        mtq.set_duty_cycle(200)
        dipole_moment = mtq.get_moment()
        expected_dipole_moment = np.array([0.0, 0.0, 0.175])
        np.testing.assert_almost_equal(dipole_moment, expected_dipole_moment, 3)

    def test_mag_model_dutycycle_lower_bound(self):
        """Saturation test of generating negative dipole moment"""
        kernel = AleasimKernel(dt=0.01)
        normal_vec = np.array([0.0, 0.0, 1.0])
        mtq = MagnetorquerModel('mtq', kernel, saturation_moment=0.2, normal_vec=normal_vec)
        kernel.add_model(mtq, True)
        mtq.power_on()

        mtq.set_duty_cycle(-200)
        dipole_moment = mtq.get_moment()
        expected_dipole_moment = np.array([0.0, 0.0, -0.175])
        np.testing.assert_almost_equal(dipole_moment, expected_dipole_moment, 3)
    
    def test_mag_model_saturate_positive(self):
        """Saturation test of generating positive dipole moment"""
        kernel = AleasimKernel(dt=0.01)
        normal_vec = np.array([0.0, 1.0, 0.0])
        mtq = MagnetorquerModel('mtq', kernel, saturation_moment=0.15, normal_vec=normal_vec)
        kernel.add_model(mtq, True)
        mtq.power_on()

        mtq.set_duty_cycle(100)
        dipole_moment = mtq.get_moment()
        expected_dipole_moment = np.array([0.0, 0.15, 0.0])
        np.testing.assert_almost_equal(dipole_moment, expected_dipole_moment, 3)
    
    def test_mag_model_saturate_negative(self):
        """Saturation test of generating negative dipole moment"""
        kernel = AleasimKernel(dt=0.01)
        normal_vec = np.array([0.0, 1.0, 0.0])
        mtq = MagnetorquerModel('mtq', kernel, saturation_moment=0.15, normal_vec=normal_vec)
        kernel.add_model(mtq, True)
        mtq.power_on()

        mtq.set_duty_cycle(-100)
        dipole_moment = mtq.get_moment()
        expected_dipole_moment = np.array([0.0, -0.15, 0.0])
        np.testing.assert_almost_equal(dipole_moment, expected_dipole_moment, 3)
    
    def test_mag_model_power_positive(self):
        """Power consumption test for magnetorquer with positive duty cycle"""
        kernel = AleasimKernel(dt=0.01)
        normal_vec = np.array([0.0, 0.0, 1.0])
        mtq = MagnetorquerModel('mtq', kernel, saturation_moment=0.15, normal_vec=normal_vec)
        kernel.add_model(mtq, True)
        mtq.power_on()

        mtq.set_duty_cycle(33)
        current = mtq.current
        power = mtq.calculate_active_power_usage()
        np.testing.assert_almost_equal(current, 0.055, 3)
        np.testing.assert_almost_equal(power, 0.0908, 3)
    
    def test_mag_model_power_negative(self):
        """Power consumption test for magnetorquer with negative duty cycle"""
        kernel = AleasimKernel(dt=0.01)
        normal_vec = np.array([0.0, 0.0, 1.0])
        mtq = MagnetorquerModel('mtq', kernel, saturation_moment=0.15, normal_vec=normal_vec)
        kernel.add_model(mtq, True)
        mtq.power_on()

        mtq.set_duty_cycle(-33)
        current = mtq.current
        power = mtq.calculate_active_power_usage()
        np.testing.assert_almost_equal(current, 0.055, 3)
        np.testing.assert_almost_equal(power, 0.0908, 3)

    def test_mag_model_power_off(self):
        """Test magnetorquer power off."""
        kernel = AleasimKernel(dt=0.01)
        normal_vec = np.array([0.0, 0.0, 1.0])
        mtq = MagnetorquerModel('mtq', kernel, saturation_moment=0.15, normal_vec=normal_vec)
        kernel.add_model(mtq, True)
        mtq.power_on()

        mtq.set_duty_cycle(100)
        
        self.assertGreater(mtq.current, 0.0)
        self.assertGreater(mtq.calculate_active_power_usage(), 0.0)
        peak_current = mtq.current
        peak_moment = mtq._saturation_moment
        
        mtq.power_off()
        self.assertEqual(mtq._dipole_moment, 0.0)
        self.assertEqual(mtq.current, 0.0)
        
        mtq.set_duty_cycle(100)
        self.assertEqual(mtq._dipole_moment, 0.0)
        self.assertEqual(mtq.current, 0.0)
        
        mtq.power_on()
        mtq.set_duty_cycle(100)
        self.assertEqual(mtq._dipole_moment, peak_moment)
        self.assertEqual(mtq.current, peak_current)

if __name__ == '__main__':
    unittest.main()
