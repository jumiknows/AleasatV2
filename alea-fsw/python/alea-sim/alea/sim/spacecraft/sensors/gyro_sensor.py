import numpy as np
import dataclasses
from pathlib import Path

from alea.sim.spacecraft.sensors.simple_sensors import SimpleSensorConfig
from .simple_sensors import Measurement
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.spacecraft.sensors.simple_sensors import SimpleSensor

@dataclasses.dataclass
class GyroSensorConfig(SimpleSensorConfig):
    """
    Gyro sensor config dataclass
    """
    noise_arw         : float = None
    power_consumption : float = None

class SimpleGyroSensor(SimpleSensor):
    """
    Simple sensor that produces 3 axis magnetic field measurement in body frame
    Adds a noise distribution based on sample rate (multiple of simulation timestep) and noise_asd
    """
    def __init__(self,
                 name: str,
                 kernel: AleasimKernel,
                 sample_rate: int,
                 seed: int = None,
                 cfg: str | Path | dict | GyroSensorConfig = "gyro_sensor"
                 ):
        super().__init__(cfg=cfg,
                         cfg_cls=GyroSensorConfig,
                         name=name,
                         sim_kernel=kernel,
                         frame=kernel.body_frame,
                         sample_rate=sample_rate,
                         seed=seed
                         )

    def connect(self):
        self._adyn: AttitudeDynamicsModel = self.kernel.get_model(AttitudeDynamicsModel)

    def measure_ideal(self) -> np.ndarray:
        v = self._adyn.angular_velocity
        return self.frame.transform_vector_from_frame(v, self.kernel.body_frame)

    def calculate_active_power_usage(self) -> float:
        return self.cfg.current_nominal * self.cfg.voltage_nominal
    
    @property
    def current(self) -> float:
        return self.cfg.current_nominal
    
class GyroSensor(SimpleGyroSensor):
    """
    Sensor model for gyro that includes additional noise characteristics.
    """
    def __init__(self,
                 name: str,
                 kernel: AleasimKernel, 
                 sample_rate: int,
                 seed : int = None):

        super().__init__(name=name, kernel=kernel, sample_rate=sample_rate, seed=seed)

        self._rate_random_walk = np.zeros(self.axes) # cumulative random walk
        
        # save additional elements for noise
        self._element_names.extend(["rrw_x", "rrw_y", "rrw_z"])
        self._element_names.extend(["arw_x", "arw_y", "arw_z"])
        self._saved_state_size = len(self._element_names)

        self._cumulative_bias_drift = np.zeros(self.axes) # cumulative bias drift

        # save time of previous measurement
        self._previous_measure_time = self.kernel.time
    
    def calibrate_rrw(self):
        """
        Zeros the built up RRW value
        """
        self._rate_random_walk = np.zeros(self.axes)

    def generate_bias(self, tdiff):
        """
        Generates sensor bias drift given a time elapsed
        """
        bdn = self.noise.generator.normal(0,1,size=self.axes)
        return self.cfg.constant_bias * np.sqrt(tdiff) * bdn
    
    def measure(self) -> Measurement:
        """
        produce sensor measurement with all unideal affects
        if the sensor is off, a null measurement ([0,0,0]) will be returned
        """
        t = self.kernel.time
        ideal = self.measure_ideal()
        
        if self.is_powered_off:
            value = np.zeros(3, dtype=np.float64)
        else:
            value = ideal
        
            #apply sensor misalignment
            value = self.misalignment_matrix @ value
            
            #add noise
            if self.noise is not None:
                noise_sample = self.noise.sample()
                self._rate_random_walk += noise_sample
                value += self._rate_random_walk

            #add bias drift (adapted from Eq. 4.40 of Landis and Markley)
            tdiff = t - self._previous_measure_time
            bd_noise = self.generate_bias(tdiff)
            self._cumulative_bias_drift += bd_noise
            
            value += self._cumulative_bias_drift
            self._previous_measure_time = t

            #add angular random walk
            awgn = self.noise.generator.normal(0,1,size=self.axes)
            arw_noise = self.cfg.noise_arw * np.sqrt(self.sample_rate) * awgn
            value += arw_noise

            #clamp to measurement range clamp and resolution
            for i in range(value.size):
                if self.measurement_range < np.inf:
                    value[i] = min(max(value[i], -self.measurement_range), self.measurement_range)
                if self.resolution > 0:
                    value[i] = self.resolution * round(value[i]/self.resolution)
        
        #save the sensor data for later processing/analysis
        saved_state = np.zeros(self.saved_state_size)
        saved_state[:3] = ideal
        saved_state[3:6] = value
        saved_state[6] = self.power_usage
        saved_state[7] = self.energy_consumed
        saved_state[8:11] = self._rate_random_walk
        saved_state[11:14] = arw_noise
        self.save_chunk_to_memory(saved_state)

        #return a measurement object
        return Measurement(t, value, self.frame)
