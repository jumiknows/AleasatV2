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
from alea.sim.spacecraft.eps.solar_panel import SolarPanelModel
from alea.sim.spacecraft.eps.power_system import PowerSystemModel
from alea.sim.spacecraft.eps.eps import EPSModel

def create_solar_test_scenario(start_date):
    kernel = AleasimKernel(dt=1e-2, date=start_date)

    adyn = AttitudeDynamicsModel(kernel)

    odyn = OrbitDynamicsModel(kernel)
    magm = EarthMagneticFieldModel(kernel)
    DModel = DisturbanceModel(kernel)

    ps = PowerSystemModel(kernel)
    solar_panels = ps.solar_panels
    eps = ps.eps

    kernel.add_model(magm)
    kernel.add_model(odyn)
    kernel.add_model(adyn)
    kernel.add_model(DModel)
    kernel.add_model(ps)
    
    return kernel

class SolarPanelsTest(unittest.TestCase):

    def test_solar_panel_power(self):
        kernel = create_solar_test_scenario(2024.2)
        ps: PowerSystemModel = kernel.get_model(PowerSystemModel)
        kernel.get_model(AttitudeDynamicsModel)._state = np.array([1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])

        #expected power gen for nominal eci quaternion at 2024.2 and default TLE is ~0.171 W
        #this was also generated based off the model
        #this test is more so to confim simulation determinism

        kernel.advance(1)

        self.assertAlmostEqual(np.sum(p.solar_power_gen for p in ps.solar_panels), 0.171, 3)

    def test_solar_panel_eclipse(self):
        #start the scenario 3000 seconds later
        #this is during an eclipse period of the orbit
        kernel = create_solar_test_scenario(2024.0)
        ps: PowerSystemModel = kernel.get_model(PowerSystemModel)
        kernel.get_model(AttitudeDynamicsModel)._state = np.array([1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
        
        kernel.step()
        self.assertFalse(kernel.get_model(OrbitDynamicsModel).is_sunlit)
        
        #advance one second more
        kernel.advance(1)
        
        #check that there is no power gen 
        self.assertAlmostEqual(np.sum(p._energy_gen_total for p in ps.solar_panels), 0.0)

class EpsTest(unittest.TestCase):

    def test_eps_solar_charging(self):
        kernel = create_solar_test_scenario(2024.0)
        ps: PowerSystemModel = kernel.get_model(PowerSystemModel)
        eps: EPSModel  = ps.eps
        
        #set init power
        eps._force_state_of_charge(0)
        
        kernel.advance(1)
        
        self.assertAlmostEqual(np.sum(p._energy_gen_total for p in ps.solar_panels)/3600.0, eps.energy)

    def test_eps_max_capacity(self):
        kernel = create_solar_test_scenario(2024.2)
        ps: PowerSystemModel = kernel.get_model(PowerSystemModel)
        eps: EPSModel  = ps.eps
        
        #set init state of charge
        eps._force_state_of_charge(100)
        start_soc = eps.state_of_charge
        
        kernel.advance(1)
        
        self.assertGreater(np.sum(p._energy_gen_total for p in ps.solar_panels), 0.0)
        #check that even though solar energy gen was not 0, the eps is at its charge limit
        self.assertAlmostEqual(eps.state_of_charge, start_soc)

    def test_eps_eclipse(self):
        kernel = create_solar_test_scenario(2024.0)
        ps: PowerSystemModel = kernel.get_model(PowerSystemModel)
        eps: EPSModel  = ps.eps
        
        #set init state of charge
        eps._force_state_of_charge(50)
        start_soc = eps.state_of_charge
        
        kernel.advance(1)
        
        #in an eclipse we will not see an increase
        self.assertAlmostEqual(eps.state_of_charge, start_soc)

    def test_eps_voltage(self):
        kernel = create_solar_test_scenario(2024.0)
        ps: PowerSystemModel = kernel.get_model(PowerSystemModel)
        eps: EPSModel  = ps.eps

        eps._force_state_of_charge(100)
        self.assertAlmostEqual(eps.voltage, 8.4)
        
        eps._force_state_of_charge(0)
        self.assertAlmostEqual(eps.voltage, 5.0)

    def test_eps_charge(self):
        kernel = create_solar_test_scenario(2024.2)
        ps: PowerSystemModel = kernel.get_model(PowerSystemModel)
        eps: EPSModel  = ps.eps

        eps._force_state_of_charge(100)
        self.assertAlmostEqual(eps.amp_hours, 6.4)

        eps._force_state_of_charge(0)
        self.assertAlmostEqual(eps.amp_hours, 0)
        
        old_ah = eps.amp_hours
        
        #solar power gen over 1 second
        kernel.advance(1)
        
        self.assertGreater(eps.amp_hours, old_ah)

if __name__ == '__main__':
    unittest.main()