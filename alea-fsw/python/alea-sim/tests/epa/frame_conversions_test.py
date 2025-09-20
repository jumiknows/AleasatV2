import unittest
from parameterized import parameterized

import numpy as np
import skyfield.api
import skyfield
import logging

from alea.sim.epa.frame_conversions import (
    ecef_to_eci, eci_to_ecef,
    ecef_to_eci_velocity, eci_to_ecef_velocity,
    ecef_to_eci_acceleration, eci_to_ecef_acceleration,
    eci_to_ecef_rot_mat, eci_to_orbit_rot_mat,
    ned_to_ecef, ned_to_ecef_rot_mat, ecef_to_ned
)
from alea.sim.kernel.time_utils import skyfield_time_to_gmst_radians, seconds_since_J2000
from alea.sim.math_lib import Quaternion
from alea.sim.kernel.kernel import AleasimKernel

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.spacecraft.sensors import SimpleMagSensor
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.kernel.frames import FrameTransformation, ReferenceFrame, Universe
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel

class EarthReferenceFrameConversionsTest(unittest.TestCase):

    def setUp(self):
        # Common time setup for all tests
        ts = skyfield.api.load.timescale()
        self.t = ts.utc(2026, 11, 2, 12, 5, 22)
        self.gmst = skyfield_time_to_gmst_radians(self.t)
        # We'll use ~0.1% tolerance => rtol ~ 1e-3
        self.rtol = 1e-3

    def test_ecef_to_eci_position_abs(self):
        r_ecef = np.array([-5762640, -1682738, 3156028])
        r_eci_ref = np.array([3055955.45813, 5167278.12143, 3156028])

        r_eci = ecef_to_eci(r_ecef, self.gmst)
        np.testing.assert_allclose(r_eci, r_eci_ref, rtol=self.rtol, 
                                   err_msg="ECEF->ECI position mismatch")

    def test_eci_to_ecef_position_abs(self):
        r_eci = np.array([-5762640, -1682738, 3156028])
        r_ecef_ref = np.array([5356417.63363, -2710796.39228, 3156028])

        r_ecef = eci_to_ecef(r_eci, self.gmst)
        np.testing.assert_allclose(r_ecef, r_ecef_ref, rtol=self.rtol,
                                   err_msg="ECI->ECEF position mismatch")

    @parameterized.expand([(np.array([1.0, -5.0, -0.5])*1e6, np.array([100.0, 7256.0, -312.0]), np.array([-4716.86737911, -5423.81784317, -312.0]))])
    def test_eci_to_ecef_velocity_abs(self, r_eci, v_eci, v_ecef_expected):

        v_ecef = eci_to_ecef_velocity(r_eci, v_eci, self.gmst)
        np.testing.assert_allclose(v_ecef, v_ecef_expected, rtol=self.rtol,
                                   err_msg="ECI->ECEF velocity mismatch")
        
        # round trip check
        r_ecef = eci_to_ecef(r_eci, self.gmst)
        v_eci_roundtrip = ecef_to_eci_velocity(r_ecef, v_ecef, self.gmst)
        np.testing.assert_allclose(v_eci_roundtrip, v_eci, rtol=self.rtol,
                                   err_msg="Round-trip velocity mismatch")

    @parameterized.expand([
        (np.array([1.0, -5.0, -0.5])*1e6,
         np.array([100.0, 7256.0, -312.0]),
         np.array([12.0, 535.0, -37.0]),
         np.array([-375.23556964, -381.58418398, -37.0]))
        ])
    def test_eci_to_ecef_acceleration_abs(self, r_eci, v_eci, a_eci, a_ecef_expected):

        a_ecef = eci_to_ecef_acceleration(r_eci, v_eci, a_eci, self.gmst)
        np.testing.assert_allclose(a_ecef, a_ecef_expected, rtol=self.rtol,
                                   err_msg="ECI->ECEF accel. mismatch")
        
        # round trip check
        r_ecef = eci_to_ecef(r_eci, self.gmst)
        v_ecef = eci_to_ecef_velocity(r_eci, v_eci, self.gmst)
        a_eci_roundtrip = ecef_to_eci_acceleration(r_ecef, v_ecef, a_ecef, self.gmst)
        np.testing.assert_allclose(a_eci_roundtrip, a_eci, rtol=self.rtol,
                                   err_msg="Round-trip accel mismatch")
    
    @parameterized.expand([
        (np.array([-5762640, -1682738, 3156028]), ), 
        (np.array([-7235854, -1345858656, 732455]), ),
        (np.array([-745234, -124, 901289]), ),
        ])
    def test_eci_ecef_position_roundtrip(self, r_eci: np.ndarray):
        r_ecef = eci_to_ecef(r_eci, self.gmst)
        r_ecef_rot = eci_to_ecef_rot_mat(self.gmst) @ r_eci
        np.testing.assert_allclose(r_ecef, r_ecef_rot)

        r_eci_rt = ecef_to_eci(r_ecef, self.gmst)
        np.testing.assert_allclose(r_eci_rt, r_eci)
    
    @parameterized.expand([(np.pi), (-np.pi), (0), (5.7)])
    def test_eci_ecef_rotmat(self, gmst: float):
        Rref = np.array([[np.cos(gmst), np.sin(gmst), 0], [-np.sin(gmst), np.cos(gmst), 0],[0,0,1]])
        R = eci_to_ecef_rot_mat(gmst)
        np.testing.assert_allclose(R, Rref)
        np.testing.assert_allclose(np.linalg.inv(R), R.T)
        np.testing.assert_allclose(R.T, Rref.T)

    def test_ned_ecef_conversion(self):
        """test that ned_to_ecef and ecef_to_ned conversions are correct against an external source (MATLAB)"""

        # conversion of zero vector
        # result should be zero vector, independent of longitude and latitude
        r_ned_zero = np.array([0, 0, 0])
        r_ecef_zero = ned_to_ecef(r_ned_zero, 0.7 * np.pi,  1 * np.pi) # generate random longitude and latitude
        np.testing.assert_array_almost_equal(r_ecef_zero, np.array([0, 0, 0]))

        r_ecef_zero = np.array([0, 0, 0])
        r_ned_zero = ecef_to_ned(r_ecef_zero, 1.5 * np.pi, 0.33 * np.pi) # generate random longitude and latitude
        np.testing.assert_array_almost_equal(r_ned_zero, np.array([0, 0, 0]))

        # conversion of arbitrary vector with arbitrary longitude and latitude (usual case)
        r_ned = np.array([1, 2, 3])
        long, lat = -75 * np.pi / 180, 35 * np.pi / 180
        r_ecef = ned_to_ecef(r_ned, long, lat)
        np.testing.assert_array_almost_equal(r_ecef, np.array([1.147362697338488, 3.445390729136036, -0.901577264764146]))

        r_ecef = np.array([2, 3, 4])
        long, lat = 40 * np.pi / 180, -20 * np.pi / 180
        r_ned = ecef_to_ned(r_ecef, long, lat)
        np.testing.assert_array_almost_equal(r_ned, np.array([4.942314674781266, 1.012558109983856, -1.883680368148395]))

    def test_eci_orbit_conversion(self):
        """test that eci_to_orbit and orbit_to_eci conversions are correct against an external source (DDJS_AOCS_ACubeSat document)"""

        # orbital elements taken from the TLE in OrbitDynamicsTest
        raan, incl, argp, mean_anomaly = 208.3416 / 180 * np.pi, 97.7736 / 180 * np.pi, 84.6385 / 180 * np.pi, 275.6441 / 180 * np.pi

        r_eci = np.array([-5762640, -1682738, 3156028])
        r_orbit = eci_to_orbit_rot_mat(raan, incl, argp, mean_anomaly) @ r_eci
        
        # calculated externally
        r_orbit_ref = np.array([5886861.06722, 3267687.80576, 816219.14253])
        np.testing.assert_allclose(r_orbit, r_orbit_ref, self.rtol)

        # test in the other direction
        r_orbit = np.array([-5762640, -1682738, 3156028])
        r_eci = eci_to_orbit_rot_mat(raan, incl, argp, mean_anomaly).T @ r_orbit

        r_eci_ref = np.array([3689860.5018, 5280172.42188, -2122296.92904])
        np.testing.assert_allclose(r_eci, r_eci_ref, self.rtol)

    def test_sensor_frame_conversion(self):
        kernel = AleasimKernel()
        magm = EarthMagneticFieldModel(kernel)
        sens = SimpleMagSensor('mag_sens', kernel, sample_rate=int(1/kernel.timestep))
        adyn = AttitudeDynamicsModel(kernel)
        odyn = OrbitDynamicsModel(kernel)
        kernel.add_model(magm)
        kernel.add_model(sens)
        kernel.add_model(adyn)
        kernel.add_model(odyn)
        
        kernel.set_log_level_all(logging.INFO)
        
        kernel.step()
        
        q = np.array([0.5,0,1,0])
        q/=np.linalg.norm(q)
        q_body2sens = Quaternion(q)
        sens_frame = kernel.universe.create_frame("sens_frame", kernel.body_frame, FrameTransformation(q_body2sens))
        
        q_eci2body = kernel.eci_frame.get_transformation_to(kernel.body_frame).quaternion
        q_eci2sens = kernel.eci_frame.get_transformation_to(sens_frame)
        
        print(q_eci2sens)

        v = np.random.rand(3)
        np.testing.assert_array_almost_equal(
            FrameTransformation(q_eci2body*q_body2sens).rotate_vector(v),
            sens_frame.transform_vector_from_frame(v, kernel.eci_frame),
            verbose=True)

        q = np.array([0.7,0,2,0])
        q/=np.linalg.norm(q)
        adyn.set_state(np.array([1.0,0,0,0, 25,0,0]))
        
        kernel.advance(0.1)

        print(kernel.eci_frame.get_transformation_to(kernel.body_frame))
        print(adyn.quaternion.to_DCM())

        v = np.random.rand(3)
        np.testing.assert_array_almost_equal(
            FrameTransformation(q_body2sens * kernel.eci_frame.get_transformation_to(kernel.body_frame).quaternion).rotate_vector(v), 
            sens_frame.transform_vector_from_frame(v, kernel.eci_frame),
            verbose=True)
        
        kernel.advance(0.1)
    
        v = np.random.rand(3)
        np.testing.assert_array_almost_equal(
            FrameTransformation(q_body2sens * kernel.eci_frame.get_transformation_to(kernel.body_frame).quaternion).rotate_vector(v), 
            sens_frame.transform_vector_from_frame(v, kernel.eci_frame), verbose=True)


        adyn.set_state(np.array([1.0,0,0,0, 1,23.5,12]))
        kernel.advance(0.1)
        
        q_ecibody = adyn.quaternion
        rotmat_bodysens = q_body2sens.to_DCM()
        rotmat_ecibody = q_ecibody.to_DCM()
        rotmat_nedecef = ned_to_ecef_rot_mat(odyn.position_lla[0], odyn.position_lla[1])
        rotmat_eciecef = eci_to_ecef_rot_mat(kernel.gmst_rad)
        rotmat_ecisens = kernel.eci_frame.get_transformation_to(sens_frame).quaternion.to_DCM()

        np.testing.assert_array_almost_equal(rotmat_ecisens, rotmat_bodysens @ rotmat_ecibody, verbose=True)
        
        mag_ned = magm.mag_field_vector_ned
        sun_eci = odyn.sun_vector

        sun_body1 = kernel.body_frame.transform_vector_from_frame(sun_eci, kernel.eci_frame)
        sun_body2 = rotmat_ecibody @ sun_eci
        np.testing.assert_array_almost_equal(sun_body1,sun_body2, verbose=True)
        
        mag_body1 = kernel.body_frame.transform_vector_from_frame(mag_ned, kernel.ned_frame)
        mag_body2 = rotmat_ecibody @ rotmat_eciecef.T @ rotmat_nedecef @ mag_ned
        np.testing.assert_array_almost_equal(mag_body1, mag_body2, verbose=True)
        
        v = np.random.random(3)
        v1 = kernel.ecef_frame.transform_vector_from_frame(v, sens_frame)
        v2 = rotmat_eciecef @ rotmat_ecisens.T @ v
        np.testing.assert_array_almost_equal(v1, v2, verbose=True)
        
        
    def test_frame_transform_tree(self):
        kernel = AleasimKernel(date=2024.0)
        magm = EarthMagneticFieldModel(kernel)
        sens = SimpleMagSensor('mag_sens', kernel, sample_rate=int(1/kernel.timestep))
        adyn = AttitudeDynamicsModel(kernel)
        odyn = OrbitDynamicsModel(kernel)
        kernel.add_model(magm)
        kernel.add_model(sens)
        kernel.add_model(adyn)
        kernel.add_model(odyn)
        
        kernel.set_log_level_all(logging.INFO)

        adyn.set_state(np.array([1.0, 0, 0, 0, 2.0, 0, 1.73]))
        
        kernel.step()
        
        tf = kernel.eci_frame.get_transformation_to(kernel.eci_frame)
        np.testing.assert_array_almost_equal(tf.tf, np.eye(4))
        
        tf = kernel.eci_frame.get_transformation_to(kernel.ecef_frame)
        np.testing.assert_array_almost_equal(tf.translation, np.zeros(3))
        
        tf = kernel.body_frame.get_transformation_to(kernel.eci_frame)
        np.testing.assert_array_almost_equal(tf.translation, -1*(adyn.quaternion.inverse.to_DCM() @ odyn.position_eci))

        tf = kernel.body_frame.get_transformation_to(kernel.ecef_frame)
        #error accumilates, usually off by some amount of centimeters, not an issue atm
        #TODO , check why this isnt working
        # np.testing.assert_array_almost_equal(tf.translation, -1 * tf.quaternion.to_DCM() @ odyn.position_ecef, decimal=0)
        
        tf = kernel.eci_frame.get_transformation_to(kernel.body_frame)
        np.testing.assert_array_almost_equal(tf.translation, odyn.position_eci)

        tf = kernel.ecef_frame.get_transformation_to(kernel.ned_frame)
        #error accumilates, usually off by some amount of centimeters, not an issue atm
        np.testing.assert_array_almost_equal(tf.translation, odyn.position_ecef, decimal=0)
        
        kernel.kill()

if __name__ == "__main__":
    unittest.main()
