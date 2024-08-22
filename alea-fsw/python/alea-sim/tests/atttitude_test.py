import unittest
from parameterized import parameterized

from datetime import datetime

import numpy as np
import time

from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.kernel.kernel import AleasimKernel

import matplotlib.pyplot as plt

import logging

class AttitudeDynamicsTest(unittest.TestCase):
    
    def test_attitude_init(self):
        #a good test of numerical physics simulations is that energy is conserved (within a tolerance)
        #assuming no input torques or disturbances/drags, the rotational kinetic energy of the CUBE should remain the same

        kernel = AleasimKernel(dt=0.01) #init kernal at the day this test was created

        adyn1 = AttitudeDynamicsModel(kernel)
        
        kernel.step()
        np.testing.assert_array_almost_equal(adyn1._state, np.array([1.0, 0, 0 ,0, 0, 0, 0]))
        
        kernel.kill()

    def test_attitude_sim_energy_conservation(self):
        #a good test of numerical physics simulations is that energy is conserved (within a tolerance)
        #assuming no input torques or disturbances/drags, the rotational kinetic energy of the CUBE should remain the same

        kernel = AleasimKernel(dt=0.01)

        default_state = np.array([1,0,0,0,1,50000.0,50])

        adyn1 = AttitudeDynamicsModel(kernel, init_state=default_state)
        adyn1.integrator_type = 'quad'

        adyn2 = AttitudeDynamicsModel(kernel, init_state=default_state)
        adyn2.integrator_type = 'euler'

        adyn3 = AttitudeDynamicsModel(kernel, init_state=default_state)
        adyn3.integrator_type = 'rk45'

        final_kes = []
        for adyn in [adyn1, adyn2, adyn3]:
            start_time = time.monotonic()
            print(f'testing attitude dynamics with {adyn._integrator_type} integrator.')
            ke_initial = adyn.calculate_rotational_kinetic_energy()
            err = []
            
            for _ in range(100):
                adyn.update()
                errabs = abs(ke_initial - adyn.calculate_rotational_kinetic_energy())
                err.append(errabs)
            
            print(f'1000 iterations of {adyn.integrator_type} took {time.monotonic() - start_time} s (+/- overhead of data collection)')
            
            errmean = np.mean(err)
            print(f'initial ke : {ke_initial}, final ke: {adyn.calculate_rotational_kinetic_energy()}')
            print('mean |error| in rotational kinetic energy', errmean)
            final_kes.append(adyn.calculate_rotational_kinetic_energy())

            #in general , local euler integration error is O(h^2) , but we're also looking at energy so we should take that into account
            #we just want error in energy conversation to be small enough that its not a worry
            assert errmean < 1e-20
            
        assert final_kes[0] == final_kes[1] and final_kes[1] == final_kes[2]

    def test_attitude_integration_timesteps_rk45(self):
        dt = 1e-4
        kernel = AleasimKernel(dt)

        default_state = np.array([1,0,0,0,1,50000.0,50])

        adyn = AttitudeDynamicsModel(kernel, init_state=default_state)

        kernel.add_model(adyn)

        adyn.integrator_type = 'rk45'
        for _ in range(5):
            kernel.advance(dt*5)
            print(f'time at end {kernel.time}')
            assert kernel.time == adyn._t_derivative, f'{kernel.time} != {adyn._t_derivative} using integrator: rk45'

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)
    unittest.main()