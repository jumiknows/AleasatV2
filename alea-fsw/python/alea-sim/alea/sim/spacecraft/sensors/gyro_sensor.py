import numpy as np

from .simple_sensors import Measurement
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
                 seed : int = None 
                 ):
        cfg = self.get_config()
        asd_noise = cfg['noise_asd']
        bias = np.array(cfg['constant_bias'])
        misalignment = np.array(cfg['axis_misalignment'])
        self._voltage = cfg['voltage_nominal']
        self._current = cfg['current_nominal']

        super().__init__(name, 
                         kernel, 
                         kernel.body_frame, 
                         sample_rate,
                         noise_asd=asd_noise, 
                         axis_misalignment=misalignment,
                         constant_bias=bias,
                         seed=seed
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
    
class GyroSensor(SimpleGyroSensor):
    """
    Sensor model for gyro that includes additional noise characteristics.
    """
    def __init__(self,
                 name: str,
                 kernel: AleasimKernel, 
                 sample_rate: int,
                 seed : int = None):
        cfg = self.get_config()

        self._noise_arw = cfg["noise_arw"] # [rad/√sec]
        super().__init__(name, kernel, sample_rate, seed=seed)

        self._rate_random_walk = np.zeros(self.axes) # cumulative random walk
        
        # save additional elements for noise
        self._element_names.extend(["rrw_x", "rrw_y", "rrw_z"])
        self._element_names.extend(["arw_x", "arw_y", "arw_z"])
        self._saved_state_size = len(self._element_names)

    @property
    def config_name(self) -> str:
        return 'gyro_sensor'
    
    def calibrate_rrw(self):
        """
        Zeros the built up RRW value
        """
        self._rate_random_walk = np.zeros(self.axes)
    
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
        
            #apply sensor misalsignment and constant bias
            value = self.misalignment_matrix @ value + self.constant_bias
            
            #add noise
            if self.noise is not None:
                noise_sample = self.noise.sample()
                self._rate_random_walk += noise_sample
                value += self._rate_random_walk

            #add angular random walk
            awgn = self.noise.generator.normal(0,1,size=self.axes)
            arw_noise = self._noise_arw * np.sqrt(self.sample_rate) * awgn
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