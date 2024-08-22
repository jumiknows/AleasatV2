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

from alea.sim.epa.frame_conversions import *
from alea.sim.kernel.time_utils import *
from alea.sim.math_lib import Quaternion
from alea.sim.math_lib.math import rot_x, rot_y, rot_z
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.kernel.time_utils import skyfield_time_to_gmst_radians
import numpy as np

import skyfield
from skyfield.api import load

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.spacecraft.sensors.simple_sensors import SimpleMagSensor
from alea.sim.epa.magnetic_field_model import EarthMagneticFieldModel
from alea.sim.kernel.frames import *
from alea.sim.api import AttitudeDynamicsModel, OrbitDynamicsModel
from alea.sim.spacecraft.spacecraft import Spacecraft

from test_utils import create_aleasim_test_kernel

from alea.sim.algorithms.attitude_determination.wahba_solutions import *

from parameterized import parameterized

class WahbaTest(unittest.TestCase):

    def test_quest_1(self):
        """simple tests for quest"""
        q = np.random.random(4)
        q = normalize(q)
        q_true = Quaternion(q)
        rotmat_true = q_true.to_DCM()
        
        ref = []
        obs = []
        for _ in range(2):
            vref = np.random.random(3)
            vobs = rotmat_true.T @ vref
            
            ref.append(vref)
            obs.append(vobs)
        ref = np.array(ref)
        obs = np.array(obs)
        
        q_est = wahba_quest(obs, ref)
        
        np.testing.assert_array_almost_equal(q_true.to_array(), np.absolute(q_est.to_array()))

    @parameterized.expand(WAHBA_ESTIMATORS)
    def test_all_wahba_estimator(self, estimator):
        """simple tests for all estimators defined in WAHBA_ESTIMATORS by expanding the list"""
        q = np.random.random(4)
        q = normalize(q)
        q_true = Quaternion(q)
        rotmat_true = q_true.to_DCM()
        
        ref = []
        obs = []
        for _ in range(2):
            vref = np.random.random(3)
            vobs = rotmat_true.T @ vref
            
            ref.append(vref)
            obs.append(vobs)
        ref = np.array(ref)
        obs = np.array(obs)

        q_est = estimator(obs, ref)
        np.testing.assert_array_almost_equal(q_true.to_array(), np.absolute(q_est.to_array()))
    
    def test_quest_sensor_scenario_no_noise(self):
        """test quest quaternion estimation with simple sensors"""
        kernel: AleasimKernel = create_aleasim_test_kernel()
        
        kernel.step()
        
        sc: Spacecraft = kernel.get_model(Spacecraft)
        magm: EarthMagneticFieldModel = kernel.get_model(EarthMagneticFieldModel)
        
        omega = np.random.random(3)
        newstate = sc._adyn._state
        newstate[4:7] = omega
        sc._adyn.set_state(newstate)

        kernel.advance(0.1)
        
        np.testing.assert_array_almost_equal(sc._adyn.angular_velocity, sc._gyro_sens.measure_ideal())
        
        #ground truth rotation matrices based on the spacecraft state and underlying transform matrix functions
        q_ecibody: Quaternion = sc._adyn.quaternion
        rotmat_ecibody = q_ecibody.to_DCM()
        rotmat_nedecef = ned_to_ecef_rot_mat(sc._orbit_dynamics.position_lonlat[0], sc._orbit_dynamics.position_lonlat[1])
        rotmat_eciecef = eci_to_ecef_rot_mat(kernel.gmst_rad)
        rotmat_nedeci = rotmat_eciecef.T @ rotmat_nedecef
        rotmat_nedbody = rotmat_ecibody @ rotmat_nedeci
        
        #sanity checks that the frame manager (which the sensors rely on) agrees with the ground truth
        np.testing.assert_array_almost_equal(rotmat_nedeci, kernel.ned_frame.get_transformation_to(kernel.eci_frame).rotation.to_DCM())
        np.testing.assert_array_almost_equal(rotmat_nedbody, kernel.ned_frame.get_transformation_to(kernel.body_frame).rotation.to_DCM())
        
        
        magtest = kernel.body_frame.transform_vector_from_frame(magm.get_mag_vector_ned(), kernel.ned_frame)
        
        #check that magnetic model and sensor agree
        np.testing.assert_array_almost_equal(magtest, sc._mag_sens.measure_ideal())
        np.testing.assert_array_almost_equal(rotmat_ecibody @ rotmat_nedeci @ magm.get_mag_vector_ned(), sc._mag_sens.measure_ideal())
        np.testing.assert_array_almost_equal(rotmat_nedbody.T @ magm.get_mag_vector_body(), magm.get_mag_vector_ned())
        
        #check that the sun model and sensor agree
        suntest = kernel.body_frame.transform_vector_from_frame(sc._orbit_dynamics.sun_vector_norm, kernel.eci_frame)
        np.testing.assert_array_almost_equal(suntest, sc._sun_sens.measure_ideal())
        np.testing.assert_array_almost_equal(q_ecibody.to_DCM() @ sc._orbit_dynamics.sun_vector_norm, suntest)
        np.testing.assert_array_almost_equal(rotmat_ecibody @ sc._orbit_dynamics.sun_vector_norm, sc._sun_sens.measure_ideal())
    
        #another sanity check for mag field conversion
        magned = magm.get_mag_vector_ned()
        magref = kernel.eci_frame.transform_vector_from_frame(magned, kernel.ned_frame)
        magobs = kernel.body_frame.transform_vector_from_frame(magned, kernel.ned_frame)
        np.testing.assert_array_almost_equal( rotmat_ecibody @ rotmat_nedeci @ magned, magobs)
        np.testing.assert_array_almost_equal( rotmat_nedeci @ magned, magref)
        sunref = sc._orbit_dynamics.sun_vector_norm
        
        magobs = sc._mag_sens.measure_ideal()
        sunobs = sc._sun_sens.measure_ideal()
        
        np.testing.assert_array_almost_equal(rotmat_ecibody @ magref, magobs)
        np.testing.assert_array_almost_equal(rotmat_ecibody @ sunref, sunobs)
        
        
        #start quest scenario
        #first update angular velocity and orientation
        newstate = sc._adyn._state
        omega = np.random.random(3) * np.random.randint(0, int(np.pi/2))
        newstate[4:7] = omega
        newstate[0:4] = Quaternion.random().to_array()
        sc._adyn.set_state(newstate)

        #run several loops for validation
        for _ in range(10):
            #advance 1s to propagate states
            # print('quest scenario test - advancing 1 second')
            kernel.advance(1)
            
            #true atttiude
            q_true = sc._adyn.quaternion

            #generate measurements and references
            magref = kernel.eci_frame.transform_vector_from_frame(magm.get_mag_vector_ned(), kernel.ned_frame)
            sunref = sc._orbit_dynamics.sun_vector_norm
            ref = np.array([magref, sunref])
            obs = np.array([sc._mag_sens.measure_ideal(), sc._sun_sens.measure_ideal()])
            
            #estimate attitude with quest
            #inverse because we are testing ref->body but quest returns body->ref
            q_est = wahba_quest(obs, ref).inverse
            
            #check the quaternion
            # TODO this sometimes fails
            # quest may return a negative version which is a valid roation but not an equal float
            # np.testing.assert_array_almost_equal(q_true.to_array(), np.absolute(q_est.to_array()))
            
            #also check the abs angular difference is close to 0 within tolerance
            ang_dif = q_est.abs_angle_diff(q_true)
            np.testing.assert_almost_equal(ang_dif, 0)
            
            print(f'quest estimated attitude within {ang_dif} degrees under no noise conditions')

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    unittest.main(verbosity=10)