import logging
import unittest
import numpy as np

from alea.sim.epa.disturbance_model import DisturbanceModel
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.math_lib import Quaternion
from alea.sim.spacecraft.sensors import SimpleMagSensor, SimpleSunSensor, SimpleGyroSensor
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel

def create_sens_test_scenario(date=2024.0):
    kernel = AleasimKernel(dt=1e-2, date=date)
    sample_rate = int(1/1e-1)

    adyn = AttitudeDynamicsModel(kernel)
    adyn._state[0:4] = Quaternion.random().to_array()
    adyn._state[4:7] = [0.5, 0.01, 0.0037]

    odyn = OrbitDynamicsModel(kernel)
    magm = EarthMagneticFieldModel(kernel)
    DModel = DisturbanceModel(kernel)

    mag_sens = SimpleMagSensor('mag_sens', kernel, sample_rate=sample_rate)
    sun_sens = SimpleSunSensor('sun_sens', kernel, sample_rate=sample_rate)
    gyro = SimpleGyroSensor('gyro', kernel, sample_rate=sample_rate)

    kernel.add_model(magm)
    kernel.add_model(odyn)
    kernel.add_model(adyn)
    kernel.add_model(DModel)
    kernel.add_model(mag_sens)
    kernel.add_model(sun_sens)
    kernel.add_model(gyro)
    
    return kernel

class SimpleSensorsTest(unittest.TestCase):

    def test_sensors_power(self):
        kernel = create_sens_test_scenario()
        mag_sens: SimpleMagSensor = kernel.get_model(SimpleMagSensor)
        nom_power = mag_sens.calculate_active_power_usage()
        
        kernel.advance(5)
        
        self.assertEqual(mag_sens.energy_consumed, 0.0)
        
        mag_sens.power_on()
        
        self.assertEqual(mag_sens.powered_state, SimpleMagSensor.PoweredState.POWERED_ON)
        
        t0 = kernel.time
        kernel.advance(5)
        
        self.assertEqual(mag_sens.power_usage, nom_power)
        self.assertAlmostEqual(mag_sens.energy_consumed, nom_power * (kernel.time - t0))
    
    def test_sensor_signal(self):
        kernel = create_sens_test_scenario(date=2024.2)
        mag_sens: SimpleMagSensor = kernel.get_model(SimpleMagSensor)
        sun_sens: SimpleSunSensor = kernel.get_model(SimpleSunSensor)
        gyro_sens: SimpleGyroSensor = kernel.get_model(SimpleGyroSensor)
        
        kernel.advance(1)
        
        np.testing.assert_array_almost_equal(mag_sens.measure_ideal(), 
                                             kernel.get_model(EarthMagneticFieldModel).mag_field_vector_body)
        np.testing.assert_array_almost_equal(sun_sens.measure_ideal(), 
            sun_sens.frame.transform_vector_from_frame(kernel.get_model(OrbitDynamicsModel).sun_vector_norm, kernel.eci_frame))
        
        np.testing.assert_array_equal(gyro_sens.measure_ideal(), kernel.get_model(AttitudeDynamicsModel)._state[4:7])

    def test_sun_sensor_signal_eclipse(self):
        #eclipse date is 2024.0 based on current TLE
        kernel = create_sens_test_scenario(date=2024.0)
        sun_sens: SimpleSunSensor = kernel.get_model(SimpleSunSensor)
        
        kernel.advance(1)

        np.testing.assert_array_almost_equal(sun_sens.measure_ideal(), 
            np.zeros(3))

if __name__ == '__main__':
    unittest.main()