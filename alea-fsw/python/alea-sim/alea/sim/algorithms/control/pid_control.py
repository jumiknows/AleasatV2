import numpy as np
from alea.sim.math_lib import Quaternion

def pd_control_torque(q_err: Quaternion, rate_err: np.ndarray, Kp = 1, Kd = 1) -> np.ndarray:
    """simple pd control torque calculator based on quaternion and rate error"""
    err_vec = q_err.to_array()[1:4]
    
    torque_desired = -1.0 * (Kp @ ((np.sign(q_err.to_array()[0])) * err_vec)) - Kd @ rate_err

    return torque_desired

def pid_control_torque(q_err: Quaternion, qintegral_err: np.ndarray, rate_err: np.ndarray, Kp = 1, Ki = 1, Kd = 1) -> np.ndarray:
    qe = q_err.to_array()
    qe_0 = qe[0]
    qe_vec = qe[1:4]
    qintegral_err_vec = qintegral_err[1:]
    
    torque_desired = -1.0 * (Kp @ (np.sign(qe_0) * qe_vec)) - Ki @ qintegral_err_vec - Kd @ rate_err
    
    return torque_desired
    