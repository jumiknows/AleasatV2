import numpy as np

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.spacecraft.sensors.simple_sensors import SimpleSensor
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel

class SimpleSunSensor(SimpleSensor):
    """
    Simple sensor that produces 3 axis sun vector measurement in body frame (normalized)
    Adds a noise distribution based on sample rate (multiple of simulation timestep) and noise_asd
    """
    def __init__(self, name: str, kernel: AleasimKernel, sample_rate, seed = None):
        cfg = self.get_config()
        rms_noise = cfg['rms_noise']
        bias = np.array(cfg['constant_bias'])
        misalignment = np.array(cfg['misalignment'])
        scaling = cfg['scaling']
        self._voltage = cfg['voltage_nominal']
        self._current = cfg['current_nominal']

        super().__init__(name, kernel, kernel.body_frame, 
                         sample_rate, 
                         noise_rms=rms_noise, 
                         axis_misalignment=misalignment,
                         constant_bias=bias,
                         scaling=scaling,
                         seed=seed
                         )
    
    @property
    def config_name(self) -> str:
        return 'sun_sensor'
    
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
        return self._current * self._voltage
    
    @property
    def current(self) -> float:
        return self._current