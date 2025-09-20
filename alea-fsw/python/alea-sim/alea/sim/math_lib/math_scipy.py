from typing import Tuple

import numpy as np

from scipy.spatial.transform import Rotation as rot

from . import math as adcs_math

from math import degrees

class ScipyQuaternion(adcs_math.Quaternion):
    def __init__(self, value: np.ndarray):
        """Initialize a quaternion object from array representation w,x,y,z"""
        self._q: rot = self._from_quat(value)

    def __mul__(self, q: adcs_math.Quaternion) -> adcs_math.Quaternion:
        return ScipyQuaternion((self._q * self._from_quat(q.to_array())).as_quat(scalar_first=True))
    
    @classmethod
    def _as_quat(cls, q: rot) -> np.ndarray:
        return q.as_quat(scalar_first=True)
    
    @classmethod
    def _from_quat(cls, q: np.ndarray) -> rot:
        return rot.from_quat(q, scalar_first=True)

    @property
    def conjugate(self) -> adcs_math.Quaternion:
        return ScipyQuaternion((self._as_quat(self._q.inv()) * self._q.magnitude()**2))

    @property
    def inverse(self) -> adcs_math.Quaternion:
        return ScipyQuaternion(self._as_quat(self._q.inv()))

    def to_array(self) -> np.ndarray:
        return self._as_quat(self._q)

    def to_axang(self) -> Tuple[np.ndarray, float]:
        v = self._as_quat(self._q)[1:4]
        w = self._as_quat(self._q)[0]
        denom = np.linalg.norm(v)
        angle = degrees(2.0*np.arctan2(denom, w))
        axis = np.zeros(3) if angle==0.0 else v/denom
        return (axis, angle)

    def to_DCM(self) -> np.ndarray:
        return self._q.as_matrix()

    def to_euler(self) -> np.ndarray:
        """x,y,z euler angles"""
        return self._q.as_euler('xyz', degrees=True)

    @classmethod
    def from_dcm(cls, dcm: np.ndarray) -> adcs_math.Quaternion:
        return ScipyQuaternion(cls._as_quat(rot.from_matrix(dcm)))
    
    @classmethod
    def from_euler(cls, euler: np.ndarray) -> adcs_math.Quaternion:
        """from xyz euler angles"""
        return ScipyQuaternion(cls._as_quat(rot.from_euler('xyz', euler)))