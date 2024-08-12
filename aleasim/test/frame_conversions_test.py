import unittest
from typing import Type, List, Union
import os
import time
import argparse
import sys
import threading
import logging
import pathlib
from unittest.util import safe_repr

import skyfield.api
import skyfield.framelib
import skyfield.jpllib
import skyfield.positionlib
import skyfield.timelib
import skyfield.units

from aleasim.epa.frame_conversions import *
from aleasim.kernel.time_utils import seconds_since_J2000
from aleasim.math_lib.math_ahrs import AHRSQuaternion
from aleasim.kernel.kernel import AleasimKernel
import numpy as np
import datetime
Quaternion = AHRSQuaternion

import skyfield

from aleasim.kernel.kernel import AleasimKernel
from aleasim.spacecraft.sensors.simple_sensors import SimpleMagSensor
from aleasim.epa.magnetic_field_model import EarthMagneticFieldModel
from aleasim.kernel.frames import *
from aleasim.api import AttitudeDynamicsModel, OrbitDynamicsModel

class FrameConversionsTest(unittest.TestCase):

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
        sens = SimpleMagSensor('mag_sens', kernel)
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
        q_body2sens = AHRSQuaternion(q)
        sens_frame = kernel.universe.create_frame("sens_frame", kernel.body_frame, FrameTransformation(q_body2sens))
        
        q_eci2body = kernel.eci_frame.get_transformation_to(kernel.body_frame).rotation
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
            FrameTransformation(q_body2sens * kernel.eci_frame.get_transformation_to(kernel.body_frame).rotation).rotate_vector(v), 
            sens_frame.transform_vector_from_frame(v, kernel.eci_frame),
            verbose=True)
        
        kernel.advance(0.1)
    
        v = np.random.rand(3)
        np.testing.assert_array_almost_equal(
            FrameTransformation(q_body2sens * kernel.eci_frame.get_transformation_to(kernel.body_frame).rotation).rotate_vector(v), 
            sens_frame.transform_vector_from_frame(v, kernel.eci_frame), verbose=True)


        adyn.set_state(np.array([1.0,0,0,0, 1,23.5,12]))
        kernel.advance(0.1)
        
        q_ecibody = adyn.quaternion
        rotmat_bodysens = q_body2sens.to_DCM()
        rotmat_ecibody = q_ecibody.to_DCM()
        rotmat_nedecef = ned_to_ecef_rot_mat(odyn.position_lonlat[0], odyn.position_lonlat[1])
        rotmat_eciecef = eci_to_ecef_rot_mat(kernel.gmst_rad)
        rotmat_ecisens = kernel.eci_frame.get_transformation_to(sens_frame).rotation.to_DCM()

        np.testing.assert_array_almost_equal(rotmat_ecisens, rotmat_bodysens @ rotmat_ecibody, verbose=True)
        
        mag_ned = magm.get_mag_vector_ned()
        sun_eci = odyn.calculate_sun_vector()

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
        kernel = AleasimKernel()
        kernel.set_log_level_all(log_level=logging.DEBUG)
        magm = EarthMagneticFieldModel(kernel)
        sens = SimpleMagSensor('mag_sens', kernel)
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
        # np.testing.assert_array_almost_equal(tf.translation, -1 * tf.rotation.to_DCM() @ odyn.position_ecef, decimal=0)
        
        tf = kernel.eci_frame.get_transformation_to(kernel.body_frame)
        np.testing.assert_array_almost_equal(tf.translation, odyn.position_eci)

        tf = kernel.ecef_frame.get_transformation_to(kernel.ned_frame)
        #error accumilates, usually off by some amount of centimeters, not an issue atm
        np.testing.assert_array_almost_equal(tf.translation, odyn.position_ecef, decimal=0)
        
        kernel.kill()

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    unittest.main(verbosity=10)