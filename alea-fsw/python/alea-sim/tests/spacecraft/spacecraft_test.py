import unittest

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.spacecraft.spacecraft import Spacecraft
from alea.sim.kernel.generic.powered_unit_model import PoweredUnitModel

class SpacecraftTest(unittest.TestCase):

    def test_spacecraft_power(self):
        """Test spacecraft initialization"""
        kernel = AleasimKernel(dt=1e-3)
        sc = Spacecraft(sim_kernel=kernel)
        kernel.add_model(sc)
        
        units = sc.get_all_children_of_type(PoweredUnitModel)
        self.assertGreater(len(units), 0)

        for unit in units:
            self.assertTrue(unit.is_powered_off)
        
        sc.power_on()

        for unit in units:
            self.assertTrue(unit.is_powered_on)
        
        sc.power_off()

        for unit in units:
            self.assertTrue(unit.is_powered_off)

if __name__ == '__main__':
    unittest.main()