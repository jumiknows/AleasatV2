import unittest

import numpy as np

from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.frame_conversions import *
from alea.sim.kernel.time_utils import *

import matplotlib.pyplot as plt

import logging

class OrbitDynamicsTest(unittest.TestCase):

    def test_orbit_2(self):
        kernel = AleasimKernel(dt='0.1')
        odyn = OrbitDynamicsModel(kernel)
        adyn = AttitudeDynamicsModel(kernel, init_state=np.array([1,0,0,0,500.0,1,0]))
        kernel.add_model(odyn, True)
        kernel.add_model(adyn, True)
        
        kernel.advance(kernel.timestep*10)
        
        reci = odyn.position_eci
        recef1 = eci_to_ecef(reci, kernel.gmst_rad)
        recef2 = eci_to_ecef(reci, kernel.gmst_rad)
        
        np.testing.assert_array_almost_equal(recef1, recef2)
        np.testing.assert_array_almost_equal(recef1, odyn.position_ecef)
        
        #test transforms
        tf_body2ecef = kernel.body_frame.get_transformation_to(kernel.ecef_frame).tf
        tf_ecef2ned = kernel.ecef_frame.get_transformation_to(kernel.ned_frame).tf
        tf_body2ecef_inv = kernel.body_frame.get_transformation_to(kernel.ecef_frame).inv_tf
        tf_body2ned = kernel.body_frame.get_transformation_to(kernel.ned_frame).tf
        
        np.testing.assert_array_almost_equal(tf_body2ned @ tf_body2ecef_inv , tf_ecef2ned)

        tf_ned2ecef = kernel.ned_frame.get_transformation_to(kernel.ecef_frame).tf
        tf_ned2eci = kernel.ned_frame.get_transformation_to(kernel.eci_frame).tf
        tf_ecef2eci = kernel.ecef_frame.get_transformation_to(kernel.eci_frame).tf
        
        np.testing.assert_array_almost_equal(tf_ecef2eci @ tf_ned2ecef, tf_ned2eci)
    
    def test_orbit_sun_vector(self):
        kernel = AleasimKernel(dt='0.1')
        odyn = OrbitDynamicsModel(kernel)
        adyn = AttitudeDynamicsModel(kernel)
        kernel.add_model(odyn, True)
        kernel.add_model(adyn, True)
        
        kernel.step()
        
        print(odyn.calculate_sun_vector())
        print(odyn.sun_vector_norm)


def plot_test():
    kernel = AleasimKernel(dt='10')
    odyn = OrbitDynamicsModel(kernel)
    kernel.add_model(odyn, True)
    kernel.advance(10000)

    fig = plt.figure()
    ax = fig.add_subplot(projection='3d')
    ax.scatter(odyn.state_array[:odyn.arr_size,0],odyn.state_array[:odyn.arr_size,1], odyn.state_array[:odyn.arr_size,2])
    ax.set_xlabel('X Label')
    ax.set_ylabel('Y Label')
    ax.set_zlabel('Z Label')
    # plt.scatter(odyn.state_array[:odyn.arr_size,0],odyn.state_array[:odyn.arr_size,1])
    plt.show()
    
if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    unittest.main(verbosity=100)