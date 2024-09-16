import numpy as np
import dataclasses
from pathlib import Path

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.configuration import Configurable
from alea.sim.kernel.generic.powered_unit_model import PoweredUnitModel

@dataclasses.dataclass
class MagnetorquerConfig:
    voltage                 :  float
    resistance              :  float
    num_turns               :  int
    spacing                 :  float
    init_length             :  float
    saturation_moment       :  float

class MagnetorquerModel(Configurable[MagnetorquerConfig], PoweredUnitModel):
    """
    Magnetorquer model that returns the dipole moment
    """   
    
    def __init__(self, name: str, sim_kernel: AleasimKernel, normal_vec: np.ndarray, cfg: str | Path | dict | MagnetorquerConfig = "magnetorquer", saturation_moment: float = None, voltage: float = None) -> None:
        super().__init__(name=name, sim_kernel=sim_kernel, cfg=cfg, cfg_cls=MagnetorquerConfig)

        self._current           = 0.0                                       # MTQ current
        self._dipole_moment     = 0.0                                       # MTQ dipole moment
        self._body_frame        = sim_kernel.body_frame                     # Dipole moment is calculated in body frame
        self._normal_vec        = normal_vec / np.linalg.norm(normal_vec)   # Normalized normal vector

        # Configure MTQ PWM voltage and saturation moment
        self.configure()
        if saturation_moment is not None:
            self._saturation_moment = saturation_moment
        if voltage is not None:
            self._voltage = voltage

    def configure(self):
        self._voltage = self.cfg.voltage
        self._saturation_moment = self.cfg.saturation_moment
    
    def set_duty_cycle(self, dutycycle):
        """Set dipole moment [Am^2]"""
        # Limit the range of dutyclcle to [-100, 100]
        if dutycycle < -100:
            dutycycle = -100
        if dutycycle > 100:
            dutycycle = 100
        self._current = (dutycycle / 100) * (self._voltage / self.cfg.resistance)
        side_length = self.cfg.init_length
        
        for _ in range(self.cfg.num_turns):
            area = side_length ** 2
            temp_moment = self._current * area
            self._dipole_moment += temp_moment
            side_length += self.cfg.spacing
        
        if self._dipole_moment < 0:
            self._dipole_moment = max(self._dipole_moment, -self._saturation_moment)
        else:
            self._dipole_moment = min(self._dipole_moment, self._saturation_moment)

    def get_moment(self) -> np.ndarray:
        """Get dipole moment [Am^2] in body frame"""
        return self._dipole_moment * self._normal_vec
    
    def calculate_active_power_usage(self):
        """Get power consumption [W]"""
        return self.cfg.resistance * self._current**2

    def current(self):
        """Get current [Amp]"""
        return np.abs(self._current)
