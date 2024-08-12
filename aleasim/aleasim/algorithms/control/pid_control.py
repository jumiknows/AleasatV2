import numpy as np
from aleasim.math_lib import Quaternion

def pd_control_torque(q_err: Quaternion, rate_err: np.ndarray, Kp = 1e-3, Kd = 1e-3) -> np.ndarray:
    """simple pd control torque calculator based on quaternion and rate error"""
    err_vec = q_err.to_array()[1:4]
    
    torque_desired = Kp * (np.sign(q_err.to_array()[0]) * err_vec) - Kd * rate_err
    
    return torque_desired