import numpy as np
import dataclasses
from pathlib import Path

from alea.sim.spacecraft.sensors.simple_sensors import SimpleSensorConfig
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.spacecraft.sensors.simple_sensors import SimpleSensor


@dataclasses.dataclass
class MagSensorConfig(SimpleSensorConfig):
    """
    Mag Sensor Config sensor config dataclass
    Doesn't introduce new configs unique from SimpleSensorConfig
    """

class SimpleMagSensor(SimpleSensor):
    """
    Simple sensor that produces 3 axis magnetic field measurement in body frame
    Adds a noise distribution based on sample rate (multiple of simulation timestep) and noise_asd
    """
    def __init__(self, 
                 name: str, 
                 kernel: AleasimKernel, 
                 sample_rate: int,
                 seed: int = None,
                 cfg: str | Path | dict | MagSensorConfig = "mag_sensor"
                 ):
        super().__init__(cfg=cfg,
                         cfg_cls=MagSensorConfig,
                         name=name,
                         sim_kernel=kernel,
                         frame=kernel.body_frame,
                         sample_rate=sample_rate,
                         seed=seed                
                         )

    def connect(self):
        self._magm: EarthMagneticFieldModel = self.kernel.get_model(EarthMagneticFieldModel)
    
    def measure_ideal(self) -> np.ndarray:
        return self._magm.mag_field_vector_body

    def calculate_active_power_usage(self) -> float:
        return self.cfg.current_nominal * self.cfg.voltage_nominal
    
    @property
    def current(self) -> float:
        return self.cfg.current_nominal
