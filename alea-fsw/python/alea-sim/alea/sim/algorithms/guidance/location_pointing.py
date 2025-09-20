import numpy as np
from alea.sim.math_lib import Quaternion, skew, cross

def calculate_rotation(p: np.ndarray, r: np.ndarray) -> Quaternion:
    """Calculate rotation from vector p->r"""
    #compute the rotation axis and normalize it
    e = cross(p, r)
    e /= np.linalg.norm(e)
    
    #compute the pricipal rotation about the pointing axis
    phi = np.arccos(np.dot(p ,r))
    
    return Quaternion.from_axang(e, phi)

def compute_location_pointing_quaternion(r_sc: np.ndarray, r_target: np.ndarray, v_sc: np.ndarray):
    """
    Compute target quaternion given spacecraft location and target location.
    The position vectors must reside in the same reference frame.
    
    The unit vector v_sc is body-fixed and represents the body axis to point towards the target location.

    Args:
        r_sc: Spacecraft position vector [X, Y, Z]

        r_target: Target position vector [X, Y, Z]

        v_sc: Spacecraft unit vector, specefies direction of the spacecraft to align to the target vector.
    
    Returns:
        Quaternion Rotation to align v_sc with the vector (r_target-r_sc)
    """
    
    #compute and normalize the vector from spacecraft to target
    r = r_target - r_sc
    r /= np.linalg.norm(r)
    
    return calculate_rotation(v_sc, r)

def compute_target_quaternion(v_target: np.ndarray, v_sc: np.ndarray):
    """
    Compute target quaternion given a target vector from spacecraft frame.

    Args:
        v_target: Target vector, does not have to be a unit vector.

        v_sc: Spacecraft unit vector, specefies direction of the spacecraft to align to the target vector.

    Returns:
        Rotation Quaternion to align v_sc to v_target
    """
    
    #compute and normalize the vector from spacecraft to target
    v_target /= np.linalg.norm(v_target)
    
    return calculate_rotation(v_sc, v_target)