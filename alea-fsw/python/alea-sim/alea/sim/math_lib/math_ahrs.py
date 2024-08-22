from typing import Tuple

import numpy as np

import ahrs

from . import math as adcs_math

class AHRSQuaternion(adcs_math.Quaternion):
    def __init__(self, value: np.ndarray):
        """order: w,x,y,z"""
        self._q = ahrs.Quaternion(value)

    def __mul__(self, q: adcs_math.Quaternion) -> adcs_math.Quaternion:
        # AHRS implementation doesn't return a Quaternion object from a multiplication
        # so we wrap the result here
        return AHRSQuaternion(self._q @ ahrs.Quaternion(q.to_array()))

    @property
    def conjugate(self) -> adcs_math.Quaternion:
        return AHRSQuaternion(self._q.conjugate)

    @property
    def inverse(self) -> adcs_math.Quaternion:
        return AHRSQuaternion(self._q.conjugate/np.linalg.norm(self._q.to_array()))

    def to_array(self) -> np.ndarray:
        return self._q.to_array()

    def to_axang(self) -> Tuple[np.ndarray, float]:
        return self._q.to_axang()

    def to_DCM(self) -> np.ndarray:
        return self._q.to_DCM()
    
    @classmethod
    def from_dcm(cls, dcm: np.ndarray) -> adcs_math.Quaternion:
        q = ahrs.Quaternion(dcm=dcm)
        return AHRSQuaternion(np.array([q.w,q.x,q.y,q.z]))

    @classmethod
    def from_euler(cls, euler: np.ndarray) -> adcs_math.Quaternion:
        raise NotImplementedError

    def to_euler(self) -> np.ndarray:
        return self._q.to_angles()
