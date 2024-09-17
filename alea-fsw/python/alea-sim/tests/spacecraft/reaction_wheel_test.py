import unittest
import numpy as np

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.spacecraft.actuators.reaction_wheel import ReactionWheelModel

class ReactionWheelTest(unittest.TestCase):

    def test_rw_power(self):
        """Power on reaction wheel and confirm torque command is received"""
        kernel = AleasimKernel(dt=1e-3)
        rw = ReactionWheelModel(kernel, 'rw', spin_axis_body=np.array([1.0, 0.0, 0.0]))
        kernel.add_model(rw)
        
        rw.set_torque_command(0.2e-3)
        
        kernel.advance(0.1)

        #should be zero since the reaction wheel is off
        self.assertAlmostEqual(rw.torque, 0.0)
        
        rw.power_on()
        rw.set_torque_command(0.2e-3)
        
        #should still be zero due to deadtime
        self.assertAlmostEqual(rw.torque, 0.0)

        kernel.advance(0.1)

        #now it should be at peak torque
        self.assertAlmostEqual(rw.torque, 0.2e-3)

    def test_rw_deadtime(self):
        """Test reaction wheel command communication delay."""
        kernel = AleasimKernel(dt=1e-3)
        rw = ReactionWheelModel(kernel, 'rw', spin_axis_body=np.array([1.0, 0.0, 0.0]))
        kernel.add_model(rw)
        
        rw.power_on()
        kernel.advance(5* 0.001)
        rw.set_torque_command(0.1e-3)

        #deadtime is 10 ms
        kernel.advance(5* 0.001)
        self.assertAlmostEqual(rw.torque, 0.0)
        
        kernel.advance(10 * 0.001)

        #now it should be at peak torque
        self.assertAlmostEqual(rw.torque, 0.1e-3)

    def test_rw_max_speed(self):
        """Test reaction wheel max speed limit shuts off the reaction wheel."""
        kernel = AleasimKernel(dt=1e-3)
        rw = ReactionWheelModel(kernel, 'rw', spin_axis_body=np.array([1.0, 0.0, 0.0]))
        kernel.add_model(rw)
        
        rw.power_on()
        rw.set_torque_command(rw.cfg.torque_saturation)

        #during this time it should reach max speed and fault (power off)
        kernel.advance(10)
        
        print(rw._torque_friction, rw.angular_velocity)

        self.assertTrue(rw.is_powered_off)
        
        last_vel = rw.angular_velocity
        last_fric = rw.torque_friction
        
        #confirm that the wheel is de-accelerating due to friction torque
        #though it will take a long time ~hours to reach 0
        kernel.advance(10)
        
        self.assertLess(np.abs(rw.angular_velocity), np.abs(last_vel))
        self.assertLess(np.abs(rw.torque_friction), np.abs(last_fric))

    def test_rw_power_off(self):
        """Test reaction wheel power off correctly sets the torque commands to 0."""
        kernel = AleasimKernel(dt=1e-3)
        rw = ReactionWheelModel(kernel, 'rw', spin_axis_body=np.array([1.0, 0.0, 0.0]))
        kernel.add_model(rw)
        
        rw.power_on()

        rw.set_torque_command(0.105e-3)

        kernel.advance(1)

        self.assertEqual(rw._torque_command, 0.105e-3)

        rw.power_off()
        self.assertEqual(rw._torque_command, 0.0)

if __name__ == '__main__':
    unittest.main()