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
        rw.cfg.enable_momentum_saturation = True #force to true for the test
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

    def test_dynamic_imbalance(self):
        """Test for reaction wheel dynamic imbalance"""
        kernel = AleasimKernel(dt=1e-3)
        rwx = ReactionWheelModel(kernel, 'rw_x', spin_axis_body=np.array([1.0, 0.0, 0.0]))
        rwy = ReactionWheelModel(kernel, 'rw_y', spin_axis_body=np.array([0.0, 1.0, 0.0]))
        rwz = ReactionWheelModel(kernel, 'rw_z', spin_axis_body=np.array([0.0, 0.0, 1.0]))
        kernel.add_model(rwx)
        kernel.add_model(rwy)
        kernel.add_model(rwz)
        rw = [rwx,rwy,rwz]

        for wheel in rw:
            # Checks if the magnitude of the static imbalance torque matches the expected torque computed using the model
            wheel._state = np.array([1980,np.pi/4])
            wheel.spin_frame.rotate_by_axang(axis=np.array([0,0,1.0]), angle=np.pi/4, relative_to=f"{wheel.name}_static_frame")
            torque_produced = np.linalg.norm(wheel.dynamic_torque)
            torque_expected = wheel.cfg.Ud*(1980**2)
            self.assertAlmostEqual(torque_produced,torque_expected,15,f"The torque produced does not equal to the torque expected in {wheel.name}")

            wheel._state = np.array([20,3*np.pi/2])
            wheel.spin_frame.rotate_by_axang(axis=np.array([0,0,1.0]), angle=3*np.pi/2, relative_to=f"{wheel.name}_static_frame")
            torque_produced = np.linalg.norm(wheel.dynamic_torque)
            torque_expected = wheel.cfg.Ud*(20**2)
            self.assertAlmostEqual(torque_produced,torque_expected,15,f"The torque produced does not equal to the torque expected in {wheel.name}")

            wheel._state = np.array([0,0])
            wheel.spin_frame.rotate_by_axang(axis=np.array([0,0,1.0]), angle=0, relative_to=f"{wheel.name}_static_frame")
            torque_produced = np.linalg.norm(wheel.dynamic_torque)
            torque_expected = 0
            self.assertEqual(torque_produced,torque_expected,f"The torque produced does not equal to the torque expected in {wheel.name}")
    
    def test_static_imbalance(self):
        """Test for reaction wheel static imbalance"""
        kernel = AleasimKernel(dt=1e-3)
        rwx = ReactionWheelModel(kernel, 'rw_x', spin_axis_body=np.array([1.0, 0.0, 0.0]))
        rwy = ReactionWheelModel(kernel, 'rw_y', spin_axis_body=np.array([0.0, 1.0, 0.0]))
        rwz = ReactionWheelModel(kernel, 'rw_z', spin_axis_body=np.array([0.0, 0.0, 1.0]))
        kernel.add_model(rwx)
        kernel.add_model(rwy)
        kernel.add_model(rwz)
        # Setting the lever arms for easier calculations
        rwx._lever_arm = np.array([1,0,0])
        rwy._lever_arm = np.array([0,1,0])
        rwz._lever_arm = np.array([0,0,1])
        kernel.step()
        rw = [rwx,rwy,rwz]

        for wheel in rw:
            # Checks if the magnitude of the static imbalance torque matches the expected torque computed using the model
            wheel._state = np.array([1980,np.pi/5])
            wheel.spin_frame.rotate_by_axang(axis=np.array([0,0,1.0]), angle=np.pi/5, relative_to=f"{wheel.name}_static_frame")
            torque_produced = np.linalg.norm(wheel.static_torque)
            torque_expected = wheel.cfg.Us*(1980**2)
            self.assertAlmostEqual(torque_produced,torque_expected,15,f"The torque produced does not equal to the torque expected in {wheel.name}")
            # The static imbalance force vector produced should be orthogonal to the axis of rotation
            self.assertEqual(np.dot(wheel.spin_axis_body,wheel.static_force),0)
            
            wheel._state = np.array([20,3*np.pi/2])
            wheel.spin_frame.rotate_by_axang(axis=np.array([0,0,1.0]), angle=3*np.pi/2, relative_to=f"{wheel.name}_static_frame")
            torque_produced = np.linalg.norm(wheel.static_torque)
            torque_expected = wheel.cfg.Us*(20**2)
            self.assertAlmostEqual(torque_produced,torque_expected,15,f"The torque produced does not equal to the torque expected in {wheel.name}")
            self.assertEqual(np.dot(wheel.spin_axis_body,wheel.static_force),0)

            wheel._state = np.array([0,0])
            wheel.spin_frame.rotate_by_axang(axis=np.array([0,0,1.0]), angle=0, relative_to=f"{wheel.name}_static_frame")
            torque_produced = np.linalg.norm(wheel.static_torque)
            torque_expected = 0
            self.assertEqual(torque_produced,torque_expected,f"The torque produced does not equal to the torque expected in {wheel.name}")
            self.assertEqual(np.dot(wheel.spin_axis_body,wheel.static_force),0)
    
    def test_frame_transformations_imbalances(self):
        """Test for frame transformations used in reaction wheel imbalances"""
        kernel = AleasimKernel(dt=1e-3)
        rwx = ReactionWheelModel(kernel, 'rw_x', spin_axis_body=np.array([1.0, 0.0, 0.0]))
        rwy = ReactionWheelModel(kernel, 'rw_y', spin_axis_body=np.array([0.0, 1.0, 0.0]))
        rwz = ReactionWheelModel(kernel, 'rw_z', spin_axis_body=np.array([0.0, 0.0, 1.0]))
        kernel.add_model(rwx)
        kernel.add_model(rwy)
        kernel.add_model(rwz)
        rw = [rwx,rwy,rwz]
        test_vecs = [np.array([1,0,0]),np.array([0,1,0]),np.array([0,0,1])]
        
        # Checks the transformation between the wheel frame to the body frame
        # Uses a unit vector pointing in +z in wheel spin frame
        z_vec = np.array([0,0,1])
        for wheel,vec in zip(rw,test_vecs):
            np.testing.assert_array_equal(wheel._calculate_rotation_matrix()@z_vec,vec)
    
    def test_wheel_frame_basis(self):
        """Test for reaction wheel frame"""
        kernel = AleasimKernel(dt=1e-3)
        rwx = ReactionWheelModel(kernel, 'rw_x', spin_axis_body=np.array([1.0, 0.0, 0.0]))
        rwy = ReactionWheelModel(kernel, 'rw_y', spin_axis_body=np.array([0.0, 1.0, 0.0]))
        rwz = ReactionWheelModel(kernel, 'rw_z', spin_axis_body=np.array([0.0, 0.0, 1.0]))
        kernel.add_model(rwx)
        kernel.add_model(rwy)
        kernel.add_model(rwz)
        rw = [rwx,rwy,rwz]

        # Checks the orthogonality of the basis vectors and whether they are orthonormal
        for wheel in rw:
            res1 = np.dot(wheel._basis[0],wheel._basis[1]) == 0
            res2 = np.dot(wheel._basis[0],wheel._basis[2]) == 0
            res3 = np.dot(wheel._basis[1],wheel._basis[2]) == 0
            overall = res1 and res2 and res3
            self.assertTrue(overall,f"Basis vectors are not orthogonal in {wheel.name}!")

            res1 = np.dot(wheel._basis[0],wheel._basis[0]) == 1
            res2 = np.dot(wheel._basis[1],wheel._basis[1]) == 1
            res3 = np.dot(wheel._basis[2],wheel._basis[2]) == 1
            overall = res1 and res2 and res3
            self.assertTrue(overall,f"Basis vectors are not orthonormal in {wheel.name}!")

if __name__ == '__main__':
    unittest.main()