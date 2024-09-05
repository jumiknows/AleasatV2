import abc
import numpy as np

from dataclasses import dataclass

from alea.sim.kernel import frames
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.kernel.kernel import AleasimKernel, SharedMemoryModelInterface

@dataclass(frozen=True)
class Measurement:
    t: float
    value: np.ndarray
    frame: frames.ReferenceFrame

    def transform_to(self, dest_frame: frames.ReferenceFrame) -> np.ndarray:
        return dest_frame.transform_vector_from_frame(self.value, self.frame)

@dataclass
class MeasurementNoise:
    amp_spectral_density: float # [measurement] / sqrt(Hz)
    sample_rate: int # [Hz]

    axes: int = 3

    def __post_init__(self):
        self._std_dev = (self.amp_spectral_density * np.sqrt(self.sample_rate))
        self.generator = np.random.default_rng()

    @property
    def std_dev(self) -> float:
        return self._std_dev

    @property
    def var(self) -> float:
        return (self.std_dev ** 2)

    @property
    def psd(self) -> float:
        return (self.amp_spectral_density ** 2)

    def sample(self) -> np.ndarray:
        return self.generator.normal(scale=self.std_dev, size=self.axes)

#generic simple sensor
class SimpleSensor(AbstractModel, SharedMemoryModelInterface):
    """Generic simple sensor that returns an intertial measurement vector (or scalar) in a certain frame of reference"""
    def __init__(self, name: str, kernel: AleasimKernel, frame: frames.ReferenceFrame, axes: int = 3, sample_rate: int = 0, noise_asd: float = None):
        super().__init__(name, kernel)
        self.frame       = frame
        self.axes        = axes
        self.sample_rate = sample_rate
        self.noise       = None
        self._element_names = [f'{self.name}_truth_{i}' for i in range(1,self.axes+1)]
        self._element_names.extend([f'{self.name}_meas_{i}' for i in range(1,self.axes+1)])

        if noise_asd is not None:
            self.noise = MeasurementNoise(noise_asd, self.sample_rate, axes=axes)

    @property
    def saved_state_size(self) -> int:
        #save ideal and ground truth
        return self.axes*2

    @property
    def saved_state_element_names(self) -> list[str]:
        #generic element names for a sensor with n axes
        return self._element_names

    @abc.abstractmethod
    def measure_ideal(self) -> np.ndarray:
        """Ground truth measurement for this sensor"""
        raise NotImplementedError()

    def measure(self) -> Measurement:
        value = self.measure_ideal()
        ideal = value
        t = self.kernel.time

        if self.noise is not None:
            noise_sample = self.noise.sample()
            value = value + noise_sample
        
        #save the sensor data for later processing/analysis
        saved_state = np.zeros(self.saved_state_size)
        saved_state[:self.axes] = ideal
        saved_state[self.axes:] = value
        self.save_chunk_to_memory(saved_state)

        #return a measurement object
        return Measurement(t, value, self.frame)
    
#simple sensor implementation
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from math import degrees

# BMX160 IMU VALUES
BMX_160_GYRO_NOISE_ASD  = 0.007 * (np.pi / 180.0)  # [rad/s/sqrt(Hz)] - Specified in datasheet as: 0.007 deg/s/sqrt(Hz)
BMX_160_ACCEL_NOISE_ASD = 180.0 * (1E-6 * 9.80665) # [m/s^2/sqrt(Hz)] - Specified in datasheet as: 180 ug/sqrt(Hz)
BMX_160_MAG_NOISE_RMS   = 600           # [nT]              - Specified in datasheet as: 0.6 uT - NOTE: This is not a spectral density, it's just a standard deviation

#ADIS16260 GYRO VALUES
ADIS_16260_GYRO_NOISE_ASD = 0.044 * (np.pi / 180.0) # [rad/s/sqrt(Hz)] - Specified in datasheet as: 0.044 deg/s/sqrt(Hz)

#made up value
SUN_SENSOR_NOISE_ASD = 1e-3

class SimpleMagSensor(SimpleSensor):
    """
    Simple sensor that produces 3 axis magnetic field measurement in body frame
    Adds a noise distribution based on sample rate (multiple of simulation timestep) and noise_asd
    """
    def __init__(self, name: str, kernel: AleasimKernel, axes: int = 3, sample_rate: int = 0, noise_asd: float = None):
        if noise_asd is None:
            noise_asd = BMX_160_MAG_NOISE_RMS / np.sqrt(sample_rate)
        super().__init__(name, kernel, kernel.body_frame, axes, sample_rate, noise_asd)
    
    @property
    def config_name(self) -> str:
        return 'magnetic_sensor'
    
    def connect(self):
        self._magm: EarthMagneticFieldModel = self.kernel.get_model(EarthMagneticFieldModel)
    
    def measure_ideal(self) -> np.ndarray:
        return self._magm.mag_field_vector_body

class SimpleSunSensor(SimpleSensor):
    """
    Simple sensor that produces 3 axis sun vector measurement in body frame (normalized)
    Adds a noise distribution based on sample rate (multiple of simulation timestep) and noise_asd
    """
    def __init__(self, name: str, kernel: AleasimKernel, axes: int = 3, sample_rate: int = 0, noise_asd: float = SUN_SENSOR_NOISE_ASD):
        super().__init__(name, kernel, kernel.body_frame, axes, sample_rate, noise_asd)
    
    @property
    def config_name(self) -> str:
        return 'sun_sensor'
    
    def connect(self):
        self._odyn: OrbitDynamicsModel = self.kernel.get_model(OrbitDynamicsModel)
    
    def measure_ideal(self) -> np.ndarray:
        sun_vec_eci_norm = self._odyn.sun_vector_norm
        
        #transform from eci to desired frame
        sun_vec_body_norm = self.frame.transform_vector_from_frame(sun_vec_eci_norm, self.kernel.eci_frame)
        
        return sun_vec_body_norm

class SimpleGyroSensor(SimpleSensor):
    """
    Simple sensor that produces 3 axis inertial->body rate vector measurement [rad/s]
    Adds a noise distribution based on sample rate (multiple of simulation timestep) and noise_asd
    """
    def __init__(self, name: str, kernel: AleasimKernel, axes: int = 3, sample_rate: int = 0, noise_asd: float = ADIS_16260_GYRO_NOISE_ASD):
        super().__init__(name, kernel, kernel.body_frame, axes, sample_rate, noise_asd)
    
    @property
    def config_name(self) -> str:
        return 'gyro_sensor'
    
    def connect(self):
        self._adyn: AttitudeDynamicsModel = self.kernel.get_model(AttitudeDynamicsModel)
    
    def measure_ideal(self) -> np.ndarray:
        return self._adyn.angular_velocity