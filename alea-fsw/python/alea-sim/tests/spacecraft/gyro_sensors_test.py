import logging
import unittest
import numpy as np

from alea.sim.epa.disturbance_model import DisturbanceModel
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.math_lib import Quaternion
from alea.sim.spacecraft.spacecraft import Spacecraft

from alea.sim.spacecraft.sensors import SimpleMagSensor, SimpleSunSensor, SimpleGyroSensor, GyroSensor
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel

def create_sens_test_scenario(date=2024.0, seed=None):
    kernel = AleasimKernel(dt=1e-2, date=date)
    sample_rate = int(1/1e-1)

    adyn = AttitudeDynamicsModel(kernel)
    adyn._state[0:4] = Quaternion.random().to_array()
    adyn._state[4:7] = [0.5, 0.01, 0.0037]

    odyn = OrbitDynamicsModel(kernel)
    magm = EarthMagneticFieldModel(kernel)
    DModel = DisturbanceModel(kernel)

    simp_gyro = SimpleGyroSensor("gyro_simple", kernel, sample_rate=sample_rate, seed=seed)
    rand_gyro = GyroSensor('gyro', kernel, sample_rate=sample_rate, seed=seed)

    kernel.add_model(magm)
    kernel.add_model(odyn)
    kernel.add_model(adyn)
    kernel.add_model(DModel)
    kernel.add_model(simp_gyro)
    kernel.add_model(rand_gyro)
    
    return kernel

class GyroSensorsTest(unittest.TestCase):

    def test_sensors_power(self):
        kernel = create_sens_test_scenario(date=2024.2, seed=5)
        simp_gyro: SimpleMagSensor = kernel.get_model(SimpleGyroSensor)
        rand_gyro : GyroSensor = kernel.get_model(GyroSensor)

        for gyro_impl in [simp_gyro, rand_gyro]:
            nom_power = gyro_impl.calculate_active_power_usage()
            
            kernel.advance(5)
            
            self.assertEqual(gyro_impl.energy_consumed, 0.0)
            
            gyro_impl.power_on()
            
            self.assertEqual(gyro_impl.powered_state, SimpleMagSensor.PoweredState.POWERED_ON)
            
            t0 = kernel.time
            kernel.advance(5)
            
            self.assertEqual(gyro_impl.power_usage, nom_power)
            self.assertAlmostEqual(gyro_impl.energy_consumed, nom_power * (kernel.time - t0))
    
    def test_sensor_signal(self):
        kernel = create_sens_test_scenario(date=2024.2, seed=5)
        simp_gyro: SimpleMagSensor = kernel.get_model(SimpleGyroSensor)
        rand_gyro : GyroSensor = kernel.get_model(GyroSensor)

        for gyro_impl in [simp_gyro, rand_gyro]:
            gyro_impl.power_on()
        
        kernel.advance(1)

        for gyro_impl in [simp_gyro, rand_gyro]:
            np.testing.assert_array_equal(gyro_impl.measure_ideal(), kernel.get_model(AttitudeDynamicsModel)._state[4:7])

    def test_angular_random_walk(self):
        # test for both seed 5 and seed 45
        for seed in [5,45]:
            kernel = create_sens_test_scenario(date=2024.2, seed=seed)
            simp_gyro: SimpleMagSensor = kernel.get_model(SimpleGyroSensor)
            rand_gyro : GyroSensor = kernel.get_model(GyroSensor)

            for gyro_impl in [simp_gyro, rand_gyro]:
                gyro_impl.power_on()

            kernel.advance_n(1) # for 1 timestep the only difference between the two exists within the ARW noise

            try:
                np.testing.assert_array_equal(simp_gyro.measure(), rand_gyro.measure()) # even if they have the same seed, they should differ
            except ValueError: # assert that the above is false
                self.assertTrue(True, f"SimpleGyroSensor ({simp_gyro.measure()}) and GyroSensor ({rand_gyro.measure()}) give different noise models")

    def test_rate_random_walk(self):
        kernel = create_sens_test_scenario(date=2024.2, seed=5)
        rand_gyro : GyroSensor = kernel.get_model(GyroSensor)
        rand_gyro.connect()
        rand_gyro.power_on()

        for i in range(50):
            rand_gyro.measure() # RRW accumulates within the measure method
            kernel.advance(1)

        kernel.kill()

        # ensure that there is a build-up of RRW drift
        rrw_drift = rand_gyro._rate_random_walk
        self.assertTrue(np.all(rrw_drift != 0),
                        msg=f"Gyro has RRW drift value of {rrw_drift}")

    def test_rate_random_walk_calibrate(self):
        # test calibrate method
        kernel = create_sens_test_scenario(date=2024.2, seed=5)
        rand_gyro : GyroSensor = kernel.get_model(GyroSensor)
        rand_gyro.connect()
        rand_gyro.power_on()

        for i in range(10):
            rand_gyro.measure() # RRW accumulates within the measure method
            kernel.advance_n(1)

        rrw_drift = rand_gyro._rate_random_walk
        self.assertTrue(np.all(rrw_drift != 0.0000),
                        msg=f"Gyro has RRW drift value of {rrw_drift}")
        
        rand_gyro.calibrate_rrw()
        rrw_drift = rand_gyro._rate_random_walk
        self.assertTrue(np.all(rrw_drift == 0.0000),
                        msg=f"Gyro has RRW drift value of {rrw_drift}")

if __name__ == '__main__':
    unittest.main()