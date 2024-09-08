import numpy as np

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.spacecraft.sensors.simple_sensors import SimpleSensor

BMX_160_MAG_NOISE_RMS   = 600           # [nT]              - Specified in datasheet as: 0.6 uT - NOTE: This is not a spectral density, it's just a standard deviation

class SimpleMagSensor(SimpleSensor):
    """
    Simple sensor that produces 3 axis magnetic field measurement in body frame
    Adds a noise distribution based on sample rate (multiple of simulation timestep) and noise_asd
    """
    def __init__(self, name: str, 
                 kernel: AleasimKernel, 
                 sample_rate: int
                 ):
        cfg = self.get_config()
        rms_noise = cfg['rms_noise']
        bias = np.array(cfg['constant_bias'])
        misalignment = np.array(cfg['misalignment'])
        self._voltage = cfg['voltage_nominal']
        self._current = cfg['current_nominal']

        super().__init__(name, 
                         kernel, 
                         kernel.body_frame, 
                         sample_rate, 
                         noise_rms=rms_noise, 
                         axis_misalignment=misalignment,
                         constant_bias=bias
                         )
    @property
    def config_name(self) -> str:
        return 'mag_sensor'
    
    def connect(self):
        self._magm: EarthMagneticFieldModel = self.kernel.get_model(EarthMagneticFieldModel)
    
    def measure_ideal(self) -> np.ndarray:
        return self._magm.mag_field_vector_body

    def calculate_active_power_usage(self) -> float:
        return self._current * self._voltage
    
    @property
    def current(self) -> float:
        return self._current