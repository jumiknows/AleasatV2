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

    def test_attitude_energy_conservation_and_residuals(self):
        """
        Test that for a torque-free attitude dynamics simulation the rotational kinetic energy is conserved.
        This test runs multiple integrators for N steps, records the instantaneous energy errors, and then
        asserts that the final energies are within a specified tolerance of the initial energy. It also
        compares the final energies across integrators.
        """
        dt = 1e-4
        kernel = AleasimKernel(dt=dt)
    
        # Define the initial state: quaternion [1, 0, 0, 0]
        # angular velocity is defined to be < 100 deg /s for this test
        default_state = np.array([1.0, 0, 0, 0, np.pi/4, -np.pi/2, np.pi/3])
        
        integrator_types = ['quad', 'euler', 'rk45', 'rk4_inline']
        results = {}
        N_steps = 1000
        for integrator in integrator_types:
            adyn = AttitudeDynamicsModel(kernel, init_state=default_state.copy())
            adyn.integrator_type = integrator
            ke_initial = adyn.calculate_rotational_kinetic_energy()
            energy_errors = []
            
            start_time = time.monotonic()
            for _ in range(N_steps):
                adyn.update()
                ke_current = adyn.calculate_rotational_kinetic_energy()
                energy_errors.append(abs(ke_initial - ke_current))
            elapsed = time.monotonic() - start_time
            
            final_ke = adyn.calculate_rotational_kinetic_energy()
            results[integrator] = {
                'final_ke': final_ke,
                'max_energy_error': np.max(energy_errors),
                'mean_energy_error': np.mean(energy_errors),
                'elapsed_time': elapsed
            }
            
            print(f"[{integrator}] dt={dt}, N_steps={N_steps}, elapsed_time={elapsed:.4f} s")
            print(f"    Initial KE: {ke_initial:.12f}, Final KE: {final_ke:.12f}")
            print(f"    Max energy error: {results[integrator]['max_energy_error']:.3e}")
            print(f"    Mean energy error: {results[integrator]['mean_energy_error']:.3e}")
            
            # Assert that energy is conserved to within a given tolerance
            self.assertTrue(
                np.isclose(ke_initial, final_ke, atol=1e-10),
                f"Energy conservation failed for {integrator}: initial {ke_initial}, final {final_ke}"
            )
        
        # Compare final energies among different integrators
        final_kes = [results[integ]['final_ke'] for integ in integrator_types]
        for i in range(len(final_kes)):
            for j in range(i + 1, len(final_kes)):
                self.assertTrue(
                    np.isclose(final_kes[i], final_kes[j], atol=1e-10),
                    f"Final energies differ between {integrator_types[i]} ({final_kes[i]}) and "
                    f"{integrator_types[j]} ({final_kes[j]})"
                )

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