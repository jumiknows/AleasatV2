# Standard Library
import json

# Third-Party Library
import numpy as np
import pandas as pd

class STKDataReader():
    """
    Read .csv file saved from STK for simulation and testing purposes.
    CSV files generated at https://gitlab.com/alea-2020/adcs/stk-python-api
    
    Datapoint names:
        * Time
        * Longitude
        * Latitude
        * Orientation q1
        * Orientation q2
        * Orientation q3
        * Orientation q4
        * Gyro Rate X
        * Gyro Rate Y
        * Gyro Rate Z
        * Accelerometer X
        * Accelerometer Y
        * Accelerometer Z
        * Magnetometer X
        * Magnetometer Y
        * Magnetometer Z
        * Mag Ref X
        * Mag Ref Y
        * Mag Ref Z
        * Sun Sensor X
        * Sun Sensor Y
        * Sun Sensor Z
        * Sun Ref X
        * Sun Ref Y
        * Sun Ref Z
    """

    def __init__(self, data_file: str, ic_file: str):
        self.data_df: pd.DataFrame = pd.read_csv(data_file)
        with open(ic_file) as file:
            self.ic_dict:dict = json.load(file)

        self._time_step_s = self.ic_dict["time_step_s"]
        self._max_duration_s = self.data_df["Time"][self.data_df.ndim-1]
        self._size = self.data_df["Time"].size
        self._ref_frame_name = self.ic_dict["ref_frame"]

    @property
    def ref_frame(self) -> str:
        """Name of the reference frame for all reference vectors"""
        return self._ref_frame_name

    @property
    def time_step(self) -> float:
        """Time step of the sim data in seconds. Also equal to the sensor sample rate."""
        return self._time_step_s

    @property
    def meas_sample_rate(self) -> float:
        """Sample rate of measurements in hz, should be recipricol of sim time step"""
        return 1.0/self._time_step_s
    
    @property 
    def max_duration(self) -> float:
        """Max duration of data capture = _time_step_s * datasize"""
        return self._max_duration_s

    @property 
    def size(self) -> int:
        """Number of datapoints, i.e. number of rows in dataframe"""
        return self._size
    
    @property
    def mag_ref_arr(self) -> np.ndarray:
        return self.data_df[['Mag Ref X', 'Mag Ref Y', 'Mag Ref Z']].to_numpy()

    @property
    def sun_ref_arr(self) -> np.ndarray:
        return self.data_df[['Sun Ref X', 'Sun Ref Y', 'Sun Ref Z']].to_numpy()

    @property
    def orientation_arr(self) -> np.ndarray:
        return self.data_df[['Orientation q4', 'Orientation q1', 'Orientation q2', 'Orientation q3']].to_numpy()

    def get_attitude_quaternion(self, t):
        """Return sim attitude quaternion as np.ndarray(w,x,y,z])
        From STK docs: 
        https://help.agi.com/stk/Subsystems/dataProviders/dataProviders.htm#html/dataProviders/Attitude_Quaternions.htm
        The attitude quaternion represents the body frame axes and is computed 
        with respect to the vehicle's central body inertial coordinate system"""
        return self.data_df.take([t//self.time_step])[['Orientation q4', 'Orientation q1', 'Orientation q2', 'Orientation q3']].to_numpy().reshape([4,])

    def get_gyro_rate(self, t):
        """Return gyro rates in x,y,z. Angular velocity of body axes with respect to intertial frame. Units in rad/s"""
        return self.data_df.take([t//self.time_step])[['Gyro Rate X', 'Gyro Rate Y', 'Gyro Rate Z']].to_numpy().reshape([3,])

    def get_accel_vector(self, t):
        """Return acceleration x,y,z. Accel vector of body axes relative to intertial frame. Units in m/s^2"""
        return self.data_df.take([t//self.time_step])[['Accelerometer X', 'Accelerometer Y', 'Accelerometer Z']].to_numpy().reshape([3,])

    def get_mag_vector_body(self, t):
        """
        Return mag vector x,y,z in body frame. Units in T.
        Field model is STK's 'SEET Magnetic Field'
        """
        return (1E-9)*self.data_df.take([t//self.time_step])[['Magnetometer X', 'Magnetometer Y', 'Magnetometer Z']].to_numpy().reshape([3,])

    def get_mag_vector_ref(self, t):
        """
        Return mag vector x,y,z in ref frame. Units in T.
        Field model is STK's 'SEET Magnetic Field'
        """
        return (1E-9)*self.data_df.take([t//self.time_step])[['Mag Ref X', 'Mag Ref Y', 'Mag Ref Z']].to_numpy().reshape([3,])   

    def get_sun_vector_body(self, t):
        """Return sun vector components x,y,z (normalized) in body frame."""
        return self.data_df.take([t//self.time_step])[['Sun Sensor X', 'Sun Sensor Y', 'Sun Sensor Z']].to_numpy().reshape([3,]) 

    def get_sun_vector_ref(self, t):
        """Return sun vector components x,y,z (normalized) in ref frame."""
        return self.data_df.take([t//self.time_step])[['Sun Ref X', 'Sun Ref Y', 'Sun Ref Z']].to_numpy().reshape([3,])
