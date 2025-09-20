from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.spacecraft.aocs import AOCSModel
from alea.sim.spacecraft.eps.power_system import PowerSystemModel
from alea.sim.kernel.generic.powered_unit_model import PoweredUnitModel

class Spacecraft(AbstractModel):

    def __init__(self, sim_kernel: AleasimKernel, ctrl_sample_period: float = 0.1) -> None:
        super().__init__(name="spacecraft", sim_kernel=sim_kernel)

        self.aocs = AOCSModel(sim_kernel=sim_kernel, ctrl_sample_period=ctrl_sample_period)
        self.power_system = PowerSystemModel(sim_kernel=sim_kernel)
        
        self.kernel.add_model(self.aocs, parent=self)
        self.kernel.add_model(self.power_system, parent=self)

    def power_on(self):
        """Power on all powered units on the spacecraft"""
        for unit in self.get_all_children_of_type(PoweredUnitModel):
            unit.power_on()

    def power_off(self):
        """Power off all powered units on the spacecraft"""
        for unit in self.get_all_children_of_type(PoweredUnitModel):
            unit.power_off()
