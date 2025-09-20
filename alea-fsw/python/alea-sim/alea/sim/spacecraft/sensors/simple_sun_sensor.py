import numpy as np
import dataclasses
from pathlib import Path

from alea.sim.spacecraft.sensors.simple_sensors import SimpleSensorConfig
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.spacecraft.sensors.simple_sensors import SimpleSensor
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel


@dataclasses.dataclass
class SunSensorConfig(SimpleSensorConfig):
    """
    Sun sensor config dataclass
    Does not introduce any extra configs
    """


class SimpleSunSensor(SimpleSensor):
    """
    Simple sensor that produces 3 axis sun vector measurement in body frame (normalized)
    Adds a noise distribution based on sample rate (multiple of simulation timestep) and noise_asd
    """
    def __init__(self, 
                 name: str, 
                 kernel: AleasimKernel, 
                 sample_rate: int, 
                 seed: int = None, 
                 cfg: str | Path | dict | SunSensorConfig = "sun_sensor"
                 ):
        super().__init__(cfg=cfg, 
                         cfg_cls=SunSensorConfig,
                         name=name, 
                         sim_kernel=kernel, 
                         frame=kernel.body_frame, 
                         sample_rate=sample_rate, 
                         seed=seed
                         )

    def connect(self):
        self._odyn: OrbitDynamicsModel = self.kernel.get_model(OrbitDynamicsModel)
    
    def measure_ideal(self) -> np.ndarray:
        if self._odyn.is_sunlit:
            sun_vec_eci_norm = self._odyn.sun_vector_norm
            
            #transform from eci to desired frame
            sun_vec_body_norm = self.frame.transform_vector_from_frame(sun_vec_eci_norm, self.kernel.eci_frame)
            
            return sun_vec_body_norm
        else:
            return np.zeros(3, dtype=np.float64)

    def calculate_active_power_usage(self) -> float:
        return self.cfg.current_nominal * self.cfg.voltage_nominal
    
    @property
    def current(self) -> float:
        return self.cfg.current_nominal
