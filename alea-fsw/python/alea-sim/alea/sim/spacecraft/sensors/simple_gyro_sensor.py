import numpy as np

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.spacecraft.sensors.simple_sensors import SimpleSensor

class SimpleGyroSensor(SimpleSensor):
    """
    Simple sensor that produces 3 axis magnetic field measurement in body frame
    Adds a noise distribution based on sample rate (multiple of simulation timestep) and noise_asd
    """
    def __init__(self, name: str, 
                 kernel: AleasimKernel, 
                 sample_rate:int,
                 ):
        cfg = self.get_config()
        asd_noise = cfg['asd_noise']
        bias = np.array(cfg['constant_bias'])
        misalignment = np.array(cfg['misalignment'])
        self._voltage = cfg['voltage_nominal']
        self._current = cfg['current_nominal']

        super().__init__(name, 
                         kernel, 
                         kernel.body_frame, 
                         sample_rate,
                         noise_asd=asd_noise, 
                         axis_misalignment=misalignment,
                         constant_bias=bias
                         )
    @property
    def config_name(self) -> str:
        return 'gyro_sensor'

    def connect(self):
        self._adyn: AttitudeDynamicsModel = self.kernel.get_model(AttitudeDynamicsModel)

    def measure_ideal(self) -> np.ndarray:
        v = self._adyn.angular_velocity
        return self.frame.transform_vector_from_frame(v, self.kernel.body_frame)

    def calculate_active_power_usage(self) -> float:
        return self._current * self._voltage
    
    @property
    def current(self) -> float:
        return self._current