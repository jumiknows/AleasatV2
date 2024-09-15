"""
Magnetic detumbling control laws
Based on chp 7.5 of F. Landis Markley, John L. Crassidis - Fundamentals of Spacecraft Attitude Determination and Control
"""

import numpy as np

def estimate_detumble_gain(incl_mag: float, Torb: float = 5600, Jmin: float = 0.0021619) -> float:
    """
    Calculate detumble gain becased on closed-dynamics of omega perpindicular to the Earth magnetic field

    Ref - eqn 7.55 Fundamentals of Spacecraft Attitude Determination and Control
    F. Landis Markley, John L. Crassidis

    Args:
        incl_mag:
            inclination of the spacecraft orbit relative to the geomagnetic equatorial plane
        Torb:
            orbital period in seconds
            ~ 5600 s for ALEASAT (450km/500km SSO)
        Jmin :
            minimum principal moment of inertia
            ~ 0.0021619 for ALEASAT (pre-deployed) based on Structure DDJF 2024-09-08
    """
    
    return 4*np.pi / Torb * (1 + np.sin(incl_mag)) * Jmin

def magnetic_detumble_control(omega: np.ndarray, B: np.ndarray, k_gain: float|np.ndarray=1) -> np.ndarray:
    """Magnetic Detumble Control Law

    Args:
        omega:
            Body angular rates vector [rad/s]
        B:
            Magnetic field vector in body frame [T]
        k_gain (optional):
            Detumbling gain
    """
    Bmag = np.linalg.norm(B)
    m = np.dot((k_gain/Bmag), np.cross(omega, B/Bmag))
    return m
    
def bdot_detumble_control(B: np.ndarray, Bdot: np.ndarray, k_gain: float|np.ndarray=1) -> np.ndarray:
    """Approximiation of magnetic detumble control law, referred to as Bdot
    
    In practical application, Bdot would be computed by a finite difference approach.
    It is reccomended to introduce a filter to mitigate the noise introduced by finite difference derivatives.

    Args:
        B:
            Magnetic field vector in body frame [T]
        Bdot:
            Derivative of magneitc field vector in body frame [T/s]
        k_gain (optional):
            Detumbling gain
    """
    Bmag = np.linalg.norm(B)
    return np.dot((-k_gain/Bmag), Bdot)

def bang_bang_bdot_detumble_control(Bdot: np.ndarray, m_max:float) -> np.ndarray:
    """Bang bang detumble control law

    Args:
        Bdot:
            Derivative of magneitc field vector in body frame [T/s]
        m_max:
            Max dipole moment of magnetic torquers [A/m^2]
    """
    return -m_max * np.sign(np.dot(np.ones(3), Bdot))