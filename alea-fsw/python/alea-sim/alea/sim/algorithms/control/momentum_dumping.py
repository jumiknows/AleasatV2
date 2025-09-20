"""
Momentum dumping control laws
Based on chp 7.5 of F. Landis Markley, John L. Crassidis - Fundamentals of Spacecraft Attitude Determination and Control
"""

import numpy as np
from alea.sim.math_lib import cross


def momentum_dumping_control(h: np.ndarray, B: np.ndarray, k_gain: float = 1.0) -> tuple[np.ndarray, np.ndarray]:
    """Momentum dumping control law to desaturate reaction wheels.

    Args:
        h:
            Reaction wheel angular momentum vector.

        B:
            Magnetic field vector in body frame [T]

        k_gain (optional):
            Momentum dumping gain

    Returns:
        Tuple of 2 vectors: rw torque and mtq moment commands for desaturatio
    """

    Bmag = np.linalg.norm(B)
    mtq_moment = k_gain/Bmag * cross(h, B/Bmag)
    
    t_rw = -1.0*cross(mtq_moment, B)
    return t_rw, mtq_moment