import numpy as np
import dataclasses
from pathlib import Path

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.kernel.kernel import SharedMemoryModelInterface
from alea.sim.kernel.generic.abstract_model import AbstractModel
from .solar_panel import SolarPanelModel
from .eps import EPSModel
from alea.sim.configuration import Configurable

@dataclasses.dataclass
class PowerSystemConfig:
    num_panels            : int
    panel_normals         : list[np.ndarray]
    panel_names           : list[str]

    def __post_init__(self):
        # convert list[list[float]] from dict into list[np.ndarray]
        self.panel_normals = [np.array(n) for n in self.panel_normals]

class PowerSystemModel(Configurable[PowerSystemConfig], AbstractModel):
    """
    The power system model consits of 1 EPS and N SolarPanelModels based on configuration.
    This model instantiates the EPS and SolarPanelModels as children and adds them to the kernel.
    """
    
    def __init__(self, sim_kernel: AleasimKernel, name: str = "power_system", cfg: str | Path | dict | PowerSystemConfig = "power_system") -> None:
        super().__init__(name=name, sim_kernel=sim_kernel, cfg=cfg, cfg_cls=PowerSystemConfig)

        # Instantiate the EPS and SolarPanelModels as children to this model 
        # and adds them to the kernel

        # we grab a quick reference to this model, but also add it as a child and add it to the kernel
        self.eps = EPSModel(self.kernel)
        # model tree now should look like root -> power system -> eps
        self.kernel.add_model(self.eps, parent= self)

        #create N solar panels based on config and also add them as children
        self.solar_panels : list[SolarPanelModel] = []
        for name, normal in zip(self.cfg.panel_names, self.cfg.panel_normals):
            sp = SolarPanelModel(self.kernel, name, normal)
            self.solar_panels.append(sp)
            self.kernel.add_model(sp, parent= self)
