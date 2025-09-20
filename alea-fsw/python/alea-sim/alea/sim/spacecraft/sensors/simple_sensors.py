import abc
import numpy as np

from dataclasses import dataclass

from alea.sim.kernel import frames
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.kernel.kernel import AleasimKernel, SharedMemoryModelInterface
from math import inf
from alea.sim.kernel.generic.powered_unit_model import PoweredUnitModel

from typing import Union

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
    seed: int | None = None

    def __post_init__(self):
        self._std_dev = (self.amp_spectral_density * np.sqrt(self.sample_rate))
        self.generator = np.random.default_rng(self.seed)

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
class SimpleSensor(SharedMemoryModelInterface, PoweredUnitModel, AbstractModel):
    """
    Generic simple sensor that returns a 3-axis intertial measurement vector in a certain frame of reference
    
    noise_asd or noise_rms : only specify one, if both are not specified it will raise an error
    axis_misalignment: a 3 element vector representing sensor axis skew as a percentage, defaults to [0,0,0] if None
    constant_bias: a 3 element vector adding constant bias to the ground truth (after misalignment), defaults to [0,0,0] if None
    measurement_range: maximum sensor reading, defaults to infinity
    resolution: resolution of sensor reading as a float, defaults to 0 (no rounding occurs)
    """
    def __init__(self, 
                 name: str, 
                 sim_kernel: AleasimKernel, 
                 frame: frames.ReferenceFrame,
                 sample_rate,
                 noise_asd: float = None,
                 noise_rms: float = None,
                 axis_misalignment: np.ndarray = None,
                 constant_bias: np.ndarray = None,
                 measurement_range: float = inf,
                 resolution: float = 0,
                 scaling: float = 1,
                 seed : int | None = None
                 ):
        super().__init__(name=name, sim_kernel=sim_kernel)
        self.frame       = frame
        self.axes        = 3 #3 axis sensor
        self.sample_rate = sample_rate
        self.noise       = None
        
        self._element_names = [f'{self.name}_truth_{i}' for i in range(1,self.axes+1)]
        self._element_names.extend([f'{self.name}_meas_{i}' for i in range(1,self.axes+1)])
        self._element_names.extend(['power_usage', 'energy_consumed'])
        self._saved_state_size = len(self._element_names)
        
        self.measurement_range = measurement_range
        self.resolution = resolution
        self.scaling = scaling
        self.seed = seed

        if noise_asd is not None:
            pass
        elif noise_rms is not None:
            noise_asd = noise_rms / np.sqrt(sample_rate)
        else:
            raise ValueError('noise_asd and noise_rms are both not specified.')
        self.noise = MeasurementNoise(noise_asd, self.sample_rate, axes=self.axes, seed=self.seed)
        
        if axis_misalignment is None:
            axis_misalignment = np.zeros(3)
        ax = axis_misalignment
        self.misalignment_matrix = np.array([[1.0, ax[1]/100.0, ax[2]/100.0],
                                              [ax[0]/100.0, 1.0, ax[2]/100.0],
                                              [ax[0]/100.0, ax[1]/100.0, 1.0]])

        if constant_bias is None:
            self.constant_bias = np.zeros(3, dtype=np.float64)
        else:
            self.constant_bias = constant_bias
            
        self.power_off()

    @property
    def saved_state_size(self) -> int:
        #save ideal and ground truth
        return self._saved_state_size

    @property
    def saved_state_element_names(self) -> list[str]:
        #generic element names for a sensor with n axes
        return self._element_names

    @abc.abstractmethod
    def measure_ideal(self) -> np.ndarray:
        """Ground truth measurement for this sensor"""
        raise NotImplementedError()

    def measure(self) -> Measurement:
        """
        produce sensor measurement with all unideal affects
        if the sensor is off, a null measurement ([0,0,0]) will be returned
        """
        t = self.kernel.time
        ideal = self.measure_ideal()
        
        #TODO what should sensors return when powered off?
        if self.is_powered_off:
            value = np.zeros(3, dtype=np.float64)
        else:
            value = ideal
        
            #apply sensor misalsignment and constant bias
            value = self.misalignment_matrix @ value + self.constant_bias
            
            #add noise
            if self.noise is not None:
                noise_sample = self.noise.sample()
                value += noise_sample

            #add scaling
            value *= self.scaling

            #clamp to measurement range clamp and resolution
            for i in range(value.size):
                if self.measurement_range < inf:
                    value[i] = min(max(value[i], -self.measurement_range), self.measurement_range)
                if self.resolution > 0:
                    value[i] = self.resolution * round(value[i]/self.resolution)
        
        #save the sensor data for later processing/analysis
        saved_state = np.zeros(self.saved_state_size)
        saved_state[:3] = ideal
        saved_state[3:6] = value
        saved_state[6] = self.power_usage
        saved_state[7] = self.energy_consumed
        self.save_chunk_to_memory(saved_state)

        #return a measurement object
        return Measurement(t, value, self.frame)
