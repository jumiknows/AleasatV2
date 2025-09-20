import numpy as np
from scipy.signal import lfilter, butter, bessel, sosfilt

class LowPassFilter():
    """
    Low pass filter that filters out high frequency noise in the onboard sensors

    The available filters are moving average filter, butterworth filter, and bessel filter
    """
    def __init__(self, filter_name: str, sample_rate: int | None = None, window_size: int | None = None, order: int | None = None, cutoff_freq: int | None = None):
        self._filter_name = filter_name
        self._sample_rate = sample_rate
        self._window_size = window_size
        self._order = order
        self._cutoff_freq = cutoff_freq

        self._data: np.ndarray = None

        if self._filter_name == 'avg' and self._window_size is None:
            raise Exception('Window size is not defined for moving average filter')
        
        if self._filter_name == 'butter':
            if self._sample_rate is None or self._order is None or self._cutoff_freq is None:
                raise Exception('Sampling rate, filter order, or cutoff frequency is not defined for butterworth filter')
        
        if self._filter_name == 'bessel':
            if self._sample_rate is None or self._order is None or self._cutoff_freq is None:
                raise Exception('Sampling rate, filter order, or cutoff frequency is not defined for bessel filter')
    
    def filter_output(self, data: np.ndarray) -> np.ndarray:
        # Ensure the signal is not empty
        if len(data) == 0:
            raise ValueError("Signal is empty, cannot apply the filter")
        
        self._data = data

        # Return the filtered signal based on the filter name
        if self._filter_name == 'avg':
            return self._filter_output_avg()
        elif self._filter_name == 'butter':
            return self._filter_output_butter()
        elif self._filter_name == 'bessel':
            return self._filter_output_bessel()
        else:
            raise Exception('Invalid filter name')
    
    def _filter_output_avg(self) -> np.ndarray:

        # Construct the filter kernel
        window_len = np.min([self._window_size, len(self._data)])
        
        filter_kernel = np.ones(window_len) / window_len

        # Slice the signal to only the relevant part (latest elements)
        recent_signal = self._data[-self._window_size:]

        # Filter each axis independently (x, y, z)
        filtered_signal = np.zeros(3)
        for i in range(3):
            filtered_axis = lfilter(filter_kernel, [1], recent_signal[:, i])
            # Get the latest value for each axis
            filtered_signal[i] = filtered_axis[-1]

        return filtered_signal
    
    def _filter_output_butter(self) -> np.ndarray:
        # Retrieve second order section coefficients
        sos = butter(self._order, self._cutoff_freq, 'lowpass', analog=False, output='sos', fs=self._sample_rate)

        # Filter each axis independently (x, y, z)
        filtered_signal = np.zeros(3)
        for i in range(3):
            filtered_axis = sosfilt(sos, self._data[:, i])
            # Get the latest value for each axis
            filtered_signal[i] = filtered_axis[-1]
        
        return filtered_signal
    
    def _filter_output_bessel(self) -> np.ndarray:
        # Retrieve second order section coefficients
        sos = bessel(self._order, self._cutoff_freq, 'lowpass', analog=False, output='sos', norm='phase', fs=self._sample_rate)

        # Filter each axis independently (x, y, z)
        filtered_signal = np.zeros(3)
        for i in range(3):
            filtered_axis = sosfilt(sos, self._data[:, i])
            # Get the latest value for each axis
            filtered_signal[i] = filtered_axis[-1]
        
        return filtered_signal
