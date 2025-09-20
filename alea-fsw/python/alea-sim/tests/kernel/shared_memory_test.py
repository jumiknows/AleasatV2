import unittest
import numpy as np
import time
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.kernel.kernel import AleasimKernel, SharedMemoryModelInterface
from alea.sim.spacecraft.spacecraft import Spacecraft
import logging
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.kernel.config_loader import load_config

from alea.sim.utils.test_scenarios import create_aleasim_test_kernel

from alea.sim.kernel.generic.abstract_model import ModelNotFoundError
class SharedMemoryTest(unittest.TestCase):
    
    def test_shared_memory_init(self):
        kernel = AleasimKernel(dt=0.1)

        adyn = AttitudeDynamicsModel(kernel)
        kernel.add_model(adyn)
        
        self.assertEqual(adyn.arr_size, 0)
        self.assertTrue(adyn.shared_memory_enabled)
        self.assertTrue(hasattr(adyn, "_state_array"))
        self.assertTrue(isinstance(adyn._state_array, np.ndarray))

    def test_shared_memory_save(self):
        kernel = AleasimKernel(dt=0.1)

        adyn = AttitudeDynamicsModel(kernel, array_length=10)
        kernel.add_model(adyn)
        
        self.assertEqual(adyn.shared_array_length, 10)
        
        kernel.advance_n(50)
        
        self.assertTrue(adyn._session_file.exists())
        self.assertGreater(adyn._session_file.stat().st_size, 0)

    def test_shared_memory_load(self):
        kernel = AleasimKernel(dt=0.1)

        adyn = AttitudeDynamicsModel(kernel, array_length=10)
        kernel.add_model(adyn)
        
        # this should save 10 times
        kernel.advance_n(100)
        
        arr = adyn.load_state_array_from_file()
        self.assertTrue(isinstance(arr, np.ndarray))
        
        #confirm concatenation works
        self.assertTrue(arr.shape, (100, adyn.saved_state_size))