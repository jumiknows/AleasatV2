from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.disturbance_model import DisturbanceModel
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.kernel.generic.abstract_model import AbstractModel

class EPAModel(AbstractModel):
    """
    Parent EPA (environment, positioning and attitude) model.
    
    Instantiates the following children models:
        - OrbitDynamicsModel
        - EarthMagneticFieldModel
        - AttitudeDynamicsModel
        - DisturbanceModel
    """

    def __init__(self, sim_kernel: AleasimKernel) -> None:
        super().__init__("epa", sim_kernel)
        
        self.odyn           = OrbitDynamicsModel(sim_kernel)
        self.adyn           = AttitudeDynamicsModel(sim_kernel)
        self.disturbances   = DisturbanceModel(sim_kernel)
        self.magm           = EarthMagneticFieldModel(sim_kernel)
        
        self.kernel.add_model(self.odyn, parent=self)
        self.kernel.add_model(self.adyn, parent=self)
        self.kernel.add_model(self.disturbances, parent=self)
        self.kernel.add_model(self.magm, parent=self)