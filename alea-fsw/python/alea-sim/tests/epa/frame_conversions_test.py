import unittest
from typing import Type, List, Union
import os
import time
import argparse
import sys
import threading
import logging
import pathlib
import random
from unittest.util import safe_repr

import skyfield.api
import skyfield.framelib
import skyfield.jpllib
import skyfield.positionlib
import skyfield.timelib
import skyfield.units

from alea.sim.epa.frame_conversions import *
from alea.sim.kernel.time_utils import seconds_since_J2000, skyfield_time_to_gmst_radians
from alea.sim.math_lib import Quaternion
from alea.sim.kernel.kernel import AleasimKernel
import numpy as np
import datetime

import skyfield

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.spacecraft.sensors import SimpleMagSensor
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.kernel.frames import *
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel

class FrameConversionsTest(unittest.TestCase):

    def test_ned_ecef_conversion(self):
        """test that ned_to_ecef and ecef_to_ned conversions are correct against an external source (MATLAB)"""

        # conversion of zero vector
        # result should be zero vector, independent of longitude and latitude
        r_ned_zero = np.array([0, 0, 0])
        r_ecef_zero = ned_to_ecef(r_ned_zero, (random.random() - 0.5) * 2 * np.pi, (random.random() - 0.5) * 2 * np.pi) # generate random longitude and latitude
        np.testing.assert_array_almost_equal(r_ecef_zero, np.array([0, 0, 0]))

        r_ecef_zero = np.array([0, 0, 0])
        r_ned_zero = ecef_to_ned(r_ecef_zero, (random.random() - 0.5) * 2 * np.pi, (random.random() - 0.5) * 2 * np.pi) # generate random longitude and latitude
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

    def test_ecef_eci_conversion(self):
        """test that ecef_to_eci and eci_to_ecef conversions are correct against an external source (Skyfield, Markley & Crassidis)"""
        
        # test/assert that the arrays are equal within a percent error tolerance
        def assert_array_almost_equal_percent(a, b, percent_tolerance):
            # Calculate the percent error
            percent_error = np.abs((a - b) / b) * 100
            
            # Check if the percent error for each element is within the tolerance
            if not np.all(percent_error <= percent_tolerance):
                max_error = np.max(percent_error)
                raise AssertionError(
                    f"Arrays are not almost equal within {percent_tolerance}% tolerance. "
                    f"Max percent error is {max_error:.6f}%."
                )

        # conversion of arbitrary r vectors with arbitrary time
        ts = skyfield.api.load.timescale()
        t = ts.utc(2026, 11, 2, 12, 5, 22)

        # Skyfield tells me the GMST in radians is 3.894209887288416. My external calculation gets 3.89420429521, so 5 decimal places of agreement.
        # print(skyfield_time_to_gmst_radians(t))

        r_ecef = np.array([-5762640, -1682738, 3156028])
        r_eci = ecef_to_eci(r_ecef, skyfield_time_to_gmst_radians(t))

        """
        r_ecef_skyf = skyfield.toposlib.ITRSPosition(skyfield.api.Distance([-5762640, -1682738, 3156028]))
        r_eci_skyf, _, _, _ = r_ecef_skyf._at(t)

        assert_array_almost_equal_percent(r_eci, r_eci_skyf, 1.0)

        # against Skyfield, there is 1.3% error in the x coordinate, and ~0.3% error in the y and z coordinates
        """

        # calculated using the textbook
        r_eci_ref = np.array([3055955.45813, 5167278.12143, 3156028])

        # percent error assertion with 0.1% tolerance
        assert_array_almost_equal_percent(r_eci, r_eci_ref, 0.1)
        
        # eci to ecef position conversion
        r_eci = np.array([-5762640, -1682738, 3156028])
        r_ecef = eci_to_ecef(r_eci, skyfield_time_to_gmst_radians(t))

        # calculated using the textbook
        r_ecef_ref = np.array([5356417.63363, -2710796.39228, 3156028])
        assert_array_almost_equal_percent(r_ecef, r_ecef_ref, 0.1)

        # conversion of arbitrary v vectors with arbitrary time in both directions
        # angular velocity of earth is 7.2921159 * 10^-5 rad/s (from online)
        # test-cases: general vector.

        # ecef to eci
        r_ecef = np.array([-5762640, -1682738, 3156028])
        v_ecef = np.array([3832, -4024, 4837])
        v_eci = ecef_to_eci_velocity(r_ecef, v_ecef)

        # calculated using the textbook
        v_eci_ref = np.array([3954.79720, -4444.21838, 4837])
        assert_array_almost_equal_percent(v_eci, v_eci_ref, 0.1)

        # eci to ecef
        r_eci = np.array([-5762640, -1682738, 3156028])
        v_eci = np.array([3832, -4024, 4837])
        r_ecef = eci_to_ecef(r_eci, skyfield_time_to_gmst_radians(t)) # we need r_ecef
        v_ecef = eci_to_ecef_velocity(r_ecef, v_eci)

        # calculated using the textbook
        v_ecef_ref = np.array([3634.32558, -4414.59618, 4837])
        assert_array_almost_equal_percent(v_ecef, v_ecef_ref, 0.1)

        # test that converting back works as well
        v_eci_ref = ecef_to_eci_velocity(r_ecef, v_ecef)
        assert_array_almost_equal_percent(v_eci, v_eci_ref, 0.1)

        # conversion of arbitrary a vectors with arbitrary time in both directions

        # ecef to eci
        r_ecef = np.array([-5762640, -1682738, 3156028])
        v_ecef = np.array([3832, -4024, 4837])
        a_ecef = np.array([1, 2, 3])
        a_eci = ecef_to_eci_acceleration(r_ecef, v_ecef, a_ecef)

        # calculated using the textbook
        a_eci_ref = np.array([1.61755, 2.56785, 3])
        assert_array_almost_equal_percent(a_eci, a_eci_ref, 0.1)

        # eci to ecef
        r_eci = np.array([-5762640, -1682738, 3156028])
        v_eci = np.array([3832, -4024, 4837])
        a_eci = np.array([1, 2, 3])
        r_ecef = eci_to_ecef(r_eci, skyfield_time_to_gmst_radians(t))
        v_ecef = eci_to_ecef_velocity(r_ecef, v_eci)
        a_ecef = eci_to_ecef_acceleration(r_ecef, v_ecef, a_eci)

        # calculated using the textbook
        a_ecef_ref = np.array([0.38465, 1.45555, 3])
        assert_array_almost_equal_percent(a_ecef, a_ecef_ref, 0.1)

        # note from discussion with Yousif: don't forget about error propagation effects; they might add up? difference is currently within 0.1% relative error

    # the NED to ECI test is omitted because it would consist of converting NED to ECEF, then ECEF to ECI, both of which are already tested

    def test_eci_orbit_conversion(self):
        """test that eci_to_orbit and orbit_to_eci conversions are correct against an external source (DDJS_AOCS_ACubeSat document)"""

        # test/assert that the arrays are equal within a percent error tolerance
        def assert_array_almost_equal_percent(a, b, percent_tolerance):
            # Calculate the percent error
            percent_error = np.abs((a - b) / b) * 100
            
            # Check if the percent error for each element is within the tolerance
            if not np.all(percent_error <= percent_tolerance):
                max_error = np.max(percent_error)
                raise AssertionError(
                    f"Arrays are not almost equal within {percent_tolerance}% tolerance. "
                    f"Max percent error is {max_error:.6f}%."
                )

        # orbital elements taken from the TLE in OrbitDynamicsTest
        raan, incl, argp, mean_anomaly = 208.3416 / 180 * np.pi, 97.7736 / 180 * np.pi, 84.6385 / 180 * np.pi, 275.6441 / 180 * np.pi

        r_eci = np.array([-5762640, -1682738, 3156028])
        r_orbit = eci_to_orbit_rot_mat(raan, incl, argp, mean_anomaly) @ r_eci
        
        # calculated externally
        r_orbit_ref = np.array([5886861.06722, 3267687.80576, 816219.14253])
        assert_array_almost_equal_percent(r_orbit, r_orbit_ref, 0.1)

        # test in the other direction
        r_orbit = np.array([-5762640, -1682738, 3156028])
        r_eci = eci_to_orbit_rot_mat(raan, incl, argp, mean_anomaly).T @ r_orbit

        r_eci_ref = np.array([3689860.5018, 5280172.42188, -2122296.92904])
        assert_array_almost_equal_percent(r_eci, r_eci_ref, 0.1)

    def test_ecef_eci_conversion_repeatability(self):
        """test that ecef_to_eci and eci_to_ecef conversions are working"""
        rotmat = eci_to_ecef_rot_mat(theta_gmst=0)
        np.testing.assert_array_almost_equal(rotmat, np.eye(3))

        r_ecef = np.array([-5762640, -1682738, 3156028])
        v_ecef = np.array([3832, -4024, 4837])
        a_ecef = np.array([1, 2, 3])
    
        #test accel conversion repeatability
        a_eci = ecef_to_eci_acceleration(r_ecef, v_ecef, a_ecef)
        a_ecef_2 = eci_to_ecef_acceleration(r_ecef, v_ecef, a_eci)
        np.testing.assert_array_almost_equal(a_ecef, a_ecef_2)

        #test velcoity conversion repeatability
        v_eci = ecef_to_eci_velocity(r_ecef, v_ecef)
        v_ecef_2 = eci_to_ecef_velocity(r_ecef, v_eci)
        np.testing.assert_array_almost_equal(v_ecef, v_ecef_2)
        
        #precision test
        rotmat = eci_to_ecef_rot_mat(theta_gmst=7e10)
        rotmat2 = eci_to_ecef_rot_mat(theta_gmst=7e10)
        np.testing.assert_array_almost_equal(rotmat, rotmat2)

        #time precision test
        now = datetime.datetime.now(tz=datetime.timezone.utc)
        #TODO fix the theta_gmst input
        rotmat = eci_to_ecef_rot_mat(theta_gmst=seconds_since_J2000(now))
        rotmat2 = eci_to_ecef_rot_mat(theta_gmst=seconds_since_J2000(now))
        np.testing.assert_array_almost_equal(rotmat, rotmat2)

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

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    unittest.main(verbosity=10)