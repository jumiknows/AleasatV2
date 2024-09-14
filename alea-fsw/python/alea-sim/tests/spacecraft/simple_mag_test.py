import logging
import unittest
import numpy as np

from alea.sim.epa.disturbance_model import DisturbanceModel
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.math_lib import Quaternion
from alea.sim.spacecraft.sensors import SimpleMagSensor
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel

def create_sens_test_scenario():
    kernel = AleasimKernel(dt=1e-2, date=2024.0)
    sample_rate = int(1/1e-1)

    adyn = AttitudeDynamicsModel(kernel)
    adyn._state[0:4] = Quaternion.random().to_array()
    adyn._state[4:7] = [0.5, 0.01, 0.0037]

    odyn = OrbitDynamicsModel(kernel)
    magm = EarthMagneticFieldModel(kernel)
    DModel = DisturbanceModel(kernel)

    mag_sens = SimpleMagSensor('mag_sens', kernel, sample_rate=sample_rate, seed = 0)

    kernel.add_model(magm)
    kernel.add_model(odyn)
    kernel.add_model(adyn)
    kernel.add_model(DModel)
    kernel.add_model(mag_sens)
    
    return kernel, sample_rate

class SimpleMagTest(unittest.TestCase):
    def testMeasurement(self):
        kernel, sample_rate = create_sens_test_scenario()
        mag_sens: SimpleMagSensor = kernel.get_model(SimpleMagSensor)
        mag_sens2 = SimpleMagSensor('mag_sens2', kernel, sample_rate=sample_rate, seed = 0)
        
        kernel.advance(1)

        mag_sens.power_on()
        mag_sens2.power_on()

        measured = (kernel.get_model(EarthMagneticFieldModel).mag_field_vector_body @ mag_sens.misalignment_matrix + mag_sens.constant_bias + mag_sens2.noise.sample()) * mag_sens.scaling

        np.testing.assert_array_almost_equal(mag_sens.measure().value, measured)

if __name__ == '__main__':
    unittest.main()