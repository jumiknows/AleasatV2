from typing import Tuple
import abc

import numpy as np

def skew(x: np.ndarray) -> np.ndarray:
    return np.array([[0, -x[2], x[1]],
                     [x[2], 0, -x[0]],
                     [-x[1], x[0], 0]])

def normalize(v: np.ndarray) -> np.ndarray:
    norm = np.linalg.norm(v)
    if norm == 0: 
       return v
    return v / norm

def cross(a: np.ndarray, b:np.ndarray) -> np.ndarray:
    return skew(a) @ b

#its well documented here
#https://www.mathworks.com/help/phased/ref/rotx.html
#https://www.mathworks.com/help/phased/ref/roty.html
#https://www.mathworks.com/help/phased/ref/rotz.html

def rot_x(a: float) -> np.ndarray:
    """Rotation matrix for CCW angle a [rad] about x axis"""
    return np.array([
        [1.0, 0.0, 0.0],
        [0.0, np.cos(a), -np.sin(a)],
        [0.0, np.sin(a), np.cos(a)]
    ])

def rot_y(b: float) -> np.ndarray:
    """Rotation matrix for angle CCW angle b [rad] about y-axis"""
    return np.array([
        [np.cos(b), 0.0, np.sin(b)],
        [0.0, 1.0, 0.0],
        [-np.sin(b), 0.0, np.cos(b)]
    ])

def rot_z(g: float) -> np.ndarray:
    """Rotation matrix for angle CCW angle g [rad] about z-axis"""
    return np.array([
        [np.cos(g), -np.sin(g), 0.0],
        [np.sin(g), np.cos(g), 0.0],
        [0.0, 0.0, 1]
    ])

class Quaternion(abc.ABC):
    @abc.abstractmethod
    def __mul__(self, q: "Quaternion") -> "Quaternion":
        raise NotImplementedError()

    @property
    @abc.abstractmethod
    def conjugate(self) -> "Quaternion":
        raise NotImplementedError()

    @property
    @abc.abstractmethod
    def inverse(self) -> "Quaternion":
        raise NotImplementedError()

    @abc.abstractmethod
    def to_array(self) -> np.ndarray:
        raise NotImplementedError()

    @abc.abstractmethod
    def to_axang(self) -> Tuple[np.ndarray, float]:
        raise NotImplementedError()

    @abc.abstractmethod
    def to_DCM(self) -> np.ndarray:
        raise NotImplementedError()
    
    @abc.abstractmethod
    def to_euler(self) -> np.ndarray:
        raise NotImplementedError()

    @classmethod
    @abc.abstractmethod
    def from_dcm(cls, dcm: np.ndarray) -> "Quaternion":
        raise NotImplementedError()
    
    @classmethod
    @abc.abstractmethod
    def from_euler(cls, euler: np.ndarray) -> "Quaternion":
        raise NotImplementedError()

    @classmethod
    def identity(cls) -> "Quaternion":
        return cls(np.array([1.0, 0.0, 0.0, 0.0]))

    @classmethod
    def from_array(cls, array: np.ndarray) -> "Quaternion":
        return cls(array)

    @classmethod
    def from_axang(cls, axis: np.ndarray, angle: float) ->  "Quaternion":
        real = np.array([np.cos(angle/2)])
        pure = np.sin(angle/2) * axis
        return cls(np.concatenate((real, pure)))

    @classmethod
    def random(cls) ->  "Quaternion":
        """generates a random quaternion as a normalized random 4-vector using numpy.random"""
        return cls(normalize(np.random.random(4)))

    def abs_angle_diff(self, q: "Quaternion") -> float:
        q_diff = self.inverse * q
        _, angle = q_diff.to_axang()
        if angle > 180:
            angle -= 360
            angle = abs(angle)
        return angle

    def __repr__(self) -> str:
        return f"{type(self).__name__} {str(self)}"

    def __str__(self) -> str:
        array = self.to_array()
        return f"[{array[0]:6.4f}, {array[1]:6.4f}, {array[2]:6.4f}, {array[3]:6.4f}]"