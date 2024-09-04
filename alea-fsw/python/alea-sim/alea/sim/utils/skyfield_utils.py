from typing import Generator
import datetime

import skyfield.timelib
import numpy as np

def time_len(t: skyfield.timelib.Time) -> int:
    """Return the length of a Time object (1 for a single time point) or the number of time points
    for a time series.
    """
    try:
        return len(t) # Throws TypeError if self.t is NOT a time series
    except TypeError:
        return 1

def values_to_np_array(t: skyfield.timelib.Time, values: list) -> np.ndarray:
    """Converts a list of values (e.g. skyfield position coordinates) to a numpy array.

    If t is a time series (and each element of values is a corresponding time-series of values)
    this function will return the appropriate multi-dimensional numpy array.
    """
    if time_len(t) > 1:
        return np.array(list(zip(*values)))
    else:
        return np.array(values)

def assert_time_match(a_name: str, a: skyfield.timelib.Time, b_name: str, b: skyfield.timelib.Time, tolerance_s: float):
    assert (abs(a.tai - b.tai) * (60*60*24)) < tolerance_s,\
           f"{a_name} ({a.tai:.20f}) does not match {b_name} ({b.tai:.20f}) within tolerance of {tolerance_s} s"

class SkyfieldTimeGen:
    """Generator class that yields skyfield time series'.
    """

    def __init__(self, start_time: skyfield.timelib.Time, timestep: int, n_timesteps: int, timescale: skyfield.timelib.Timescale):
        """Initializes a new SkyfieldTimeGen instance.

        Args:
            start_time: The very first Time to yield from the argument generator
            timestep: The timestep in seconds to use between times
            n_timesteps: The number of time to yield in a single Time object (i.e. the batch size)
            timescale: A skyfield timescale to use to create new skyfield Time objects
        """
        self._start_time = start_time
        self._timescale = timescale
        self._timestep = timestep
        self._n_timesteps = n_timesteps

    def __iter__(self) -> Generator[skyfield.timelib.Time, None, None]:
        start_time = self._start_time
        while True:
            # Create a time series for n_timesteps
            (year, month, day, hour, minute, second) = start_time.tai_calendar() # Use TAI since we need a uniform time-scale (UTC has jumps due to leap seconds)
            delta_seconds = (self._n_timesteps * self._timestep)
            end_second = second + delta_seconds
            time_series = self._timescale.tai(year, month, day, hour, minute, np.arange(second, end_second, self._timestep))

            yield time_series

            # Advance time
            start_time += datetime.timedelta(seconds=delta_seconds)
