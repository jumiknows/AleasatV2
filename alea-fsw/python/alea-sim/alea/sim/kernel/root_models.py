from .generic.abstract_model import AbstractModel
from .kernel import AleasimKernel

from alea.sim.epa.epa import EPAModel
from alea.sim.spacecraft.spacecraft import Spacecraft

class AleasimRootModel(AbstractModel):
    """
    Root model of a useful aleasim.
    
    Instantiates the following children models:
        - EPAModel
        - Spacecraft
    """

    def __init__(self, sim_kernel: AleasimKernel) -> None:
        super().__init__('aleasim_root', sim_kernel)
        
        self.epa = EPAModel(sim_kernel)
        self.spacecraft = Spacecraft(sim_kernel)
        
        sim_kernel.add_model(self.epa, parent=self)
        sim_kernel.add_model(self.spacecraft, parent=self)