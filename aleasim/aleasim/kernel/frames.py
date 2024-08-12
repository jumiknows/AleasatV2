"""
Handle reference frame transformations, and frame trees

Original author: Noah Tajwar
Modified by: Yousif El-Wishahy 
    for Aleasim to support transform matrices and translations
    
ALEASAT 2024
"""

from typing import Type, Union, Dict, List, Set
from dataclasses import dataclass
import weakref

import numpy as np

from aleasim.math_lib import math as adcs_math
from aleasim.math_lib import Quaternion

@dataclass
class FrameTransformation:
    rotation: adcs_math.Quaternion
    translation: np.ndarray = None

    def __post_init__(self):
        if self.translation is None:
            self.translation = np.zeros(3)
        self.tf = self._create_transformation_matrix()
        self.inv_tf = self._create_transformation_matrix_inverse()

    def _create_transformation_matrix(self) -> np.ndarray :
        dcm = self.rotation.to_DCM() #rotation matrix
        tf = np.zeros((4,4)) #homogenous transform matrix
        tf[0:3,0:3] = dcm
        tf[0:3, 3] = self.translation
        tf[3,3] = 1
        return tf
    
    def _create_transformation_matrix_inverse(self) -> np.ndarray :
        inv_rot = self.rotation.to_DCM().T #rotation matrix inverse = transpose
        inv_translation = -1*(inv_rot @ self.translation)
        tf = np.zeros((4,4)) #homogenous transform matrix
        tf[0:3,0:3] = inv_rot
        tf[0:3, 3] = inv_translation
        tf[3,3] = 1
        return tf

    @classmethod
    def identity(cls) -> "FrameTransformation":
        """Identity frame transformation. (no rotation or translation)"""
        return FrameTransformation(Quaternion.from_dcm(np.eye(3)), np.zeros(3))
    
    @classmethod
    def from_tf_mat(cls, value: np.ndarray) -> "FrameTransformation":
        r = value[0:3, 0:3]
        t = value[0:3, 3]
        return FrameTransformation(Quaternion.from_dcm(r), t)

    @classmethod
    def random(cls, translation_max_magnitude: int = 1000) -> "FrameTransformation":
        """generate random transformation that is useful for testing"""
        return FrameTransformation(Quaternion.random(), np.random.random(3)*np.random.randint(1, translation_max_magnitude))

    def rotate_by_axang(self, axis: np.ndarray, angle: float):
        q = type(self.rotation).from_axang(axis, angle)
        return self.rotate_by_q(q)

    def rotate_by_q(self, q: adcs_math.Quaternion) -> "FrameTransformation":
        next_tf = np.zeros((4,4))
        next_tf[0:3, 0:3] = q.to_DCM()
        next_tf[3,3] = 1
        tf = self.transformation_matrix() @ next_tf
        return FrameTransformation.from_tf_mat(tf)
    
    def inverse(self) -> "FrameTransformation":
        return self.from_tf_mat(self.inv_transformation_matrix())

    def transformation_matrix(self) -> np.ndarray:
        return self.tf

    def inv_transformation_matrix(self) -> np.ndarray:
        return self.inv_tf

    def rotate_vector(self, vector: np.ndarray) -> np.ndarray:
        """
        Applies a rotation to the vector where vt = Rotation * v
        """
        tf = self.tf
        v = np.zeros(4)
        v[0:3] = vector
        vt = tf @ v
        return vt[0:3]

    def inverse_rotate_vector(self, vector: np.ndarray) -> np.ndarray:
        """
        Applies a rotation to the vector where vt = Rotation * v
        """
        tf = self.inv_transformation_matrix()
        v = np.zeros(4)
        v[0:3] = vector
        vt = tf @ v
        return vt[0:3]

    def transform_coordinate(self, coordinate: np.ndarray) -> np.ndarray:
        """
        Applies a homogenous transformation to the coordinate where pt = Rotation * p + Translation
        """
        tf = self.transformation_matrix()
        p = np.ones(4)
        p[0:3] = coordinate
        pt = tf @ p
        return pt[0:3]

    def inverse_transform_coordinate(self, coordinate: np.ndarray) -> np.ndarray:
        """
        Applies a homogenous transformation to the coordinate where pt = Rotation * p + Translation
        """
        tf = self.inv_transformation_matrix()
        p = np.ones(4)
        p[0:3] = coordinate
        pt = tf @ p
        return pt[0:3]

    def __repr__(self) -> str:
        return f'FrameTransformation(rotation={self.rotation.__repr__()}, translation={self.translation.__repr__()},{self.tf.__str__()}'

    def __str__(self) -> str:
        return self.tf.__str__()
    
    def __mul__(self, tf_rhs: "FrameTransformation") -> "FrameTransformation":
        return FrameTransformation.from_tf_mat((self.tf @ tf_rhs.tf))

    #because transformation matrix multiplications do not commute
    def __rmul__(self, tf_lhs: "FrameTransformation") -> "FrameTransformation":
        return FrameTransformation.from_tf_mat((tf_lhs.tf @ self.tf))

    def __matmul__(self, other: "FrameTransformation"):
        return FrameTransformation.from_tf_mat((self.tf @ other.tf))
    
    def __imatmul__(self, other: "FrameTransformation"):
        return self.__matmul__(other)
    
    def __rmatmul__(self, other: "FrameTransformation"):
        return FrameTransformation.from_tf_mat((other.tf @ self.tf))

class Universe:
    def __init__(self, q_class: Type[adcs_math.Quaternion]):
        self._q_class = q_class

        self._frames: Dict[str, "ReferenceFrame"] = {}

    def get_frame(self, name: str) -> "ReferenceFrame":
        try:
            return self._frames[name]
        except KeyError:
            return None

    def create_frame(self, name: str, derived_from: Union["ReferenceFrame", str] = None, transformation: FrameTransformation = None) -> "ReferenceFrame":
        if name in self._frames:
            raise ValueError(f"Frame with name \"{name}\" already exists")

        new_frame = ReferenceFrame(name, self)
        self._frames[name] = new_frame

        if derived_from is not None:
            new_frame.derive_from(derived_from, transformation)

        return new_frame

    def identity_transform(self) -> FrameTransformation:
        return FrameTransformation(self._q_class.identity(), np.zeros(3))

class ReferenceFrame:
    def __init__(self, name: str, universe: Universe):
        self._name = name

        # Store a weak reference to the universe.
        # The universe holds references to all the frames. If the universe's own frames are the only objects
        # still holding onto references to the universe, then the universe can be freed.
        self._universe: Universe = weakref.proxy(universe)

        # A set of transformations to go from this frame to other frames
        self._relations: Dict[str, FrameTransformation] = {}

    @property
    def name(self) -> str:
        return self._name

    @property
    def related_frames(self) -> Set[str]:
        return self._relations.keys()
    
    def get_transformation_to(self, dest_frame: Union["ReferenceFrame", str], visited: List[str] = None) -> FrameTransformation:
        """
        Get transformation from src_frame to this frame, if such a transformation is defined
        by the relationships between self, frame and any intermediate frames.
        """

        if not isinstance(dest_frame, ReferenceFrame):
            dest_frame = self._universe.get_frame(dest_frame)

        # To find a path between the two frames, we perform a depth-first search of the frame graph,
        # starting from this frame and ending at src_frame (if such a path exists).
        # As the recursion unwinds, we chain transformations by multiplying the matrices.

        # Base case of the recursion, we've reached src_frame
        if self.name == dest_frame.name:
            return FrameTransformation.identity()
        
        if dest_frame.name in self.related_frames:
            return self._relations[dest_frame.name]
        elif self.name in dest_frame.related_frames:
            return dest_frame._relations[self.name].inv_tf

        # Since the frame graph is cyclic, keep track of frames we've already considered
        if visited is None:
            visited = []
        visited.append(self.name)

        # Iterate over the frames related to this frame
        for frame_name in self.related_frames:
            # Skip frames we've already seen
            if frame_name in visited:
                continue

            # Try this path
            frame = self._universe.get_frame(frame_name)
            tf = frame.get_transformation_to(dest_frame, visited)

            # Discard paths that did not lead to dest_frame
            if tf is None:
                continue

            # If we made it here, it means we found src_frame in this path!
            # Start chaining the transformations.
            return tf * self._relations[frame_name]

        # If we made it here it means none of the unvisited paths starting from this frame lead to src_frame
        return None

    def derive_from(self, frame: Union["ReferenceFrame", str], transformation: FrameTransformation = None):
        """Create a relationship between this frame and the provided frame.
        Applying the provided transformation to the provided frame should result in this frame.
        """

        if transformation is None:
            # NOP transformation
            transformation = self._universe.identity_transform()

        if not isinstance(frame, ReferenceFrame):
            frame = self._universe.get_frame(frame)

        self._relations[frame.name] = transformation.inverse()
        frame._relations[self.name] = transformation

    def _check_directly_related(self, frame: Union["ReferenceFrame", str]) -> "ReferenceFrame":
        if not isinstance(frame, ReferenceFrame):
            frame = self._universe.get_frame(frame)

        if frame.name not in self._relations:
            raise ValueError(f"Frame \"{self.name}\" is not directly related to \"{frame.name}\" frame")

        return frame

    def rotate_by_axang(self, axis: np.ndarray, angle: float, relative_to: Union["ReferenceFrame", str]):
        relative_to = self._check_directly_related(relative_to)

        old_transformation: FrameTransformation = relative_to.get_transformation_to(self)
        new_transformation = old_transformation.rotate_by_axang(axis, angle)
        self.derive_from(relative_to, new_transformation)

    def rotate_by_q(self, q: adcs_math.Quaternion, relative_to: Union["ReferenceFrame", str]):
        relative_to = self._check_directly_related(relative_to)

        old_transformation = relative_to.get_transformation_to(self)
        new_transformation = old_transformation.rotate_by_q(q)
        self.derive_from(relative_to, new_transformation)
    
    def transform_vector_from_frame(self, vector: np.ndarray, src_frame: Union["ReferenceFrame", str], visited: List[str] = None) -> np.ndarray:
        """Transforms the provided vector from the provided src_frame to this frame, if such a transformation is defined
        by the relationships between self, frame and any intermediate frames.
        """
        return src_frame.get_transformation_to(self).rotate_vector(vector)

    def transform_position_from_frame(self, pos: np.ndarray, src_frame: Union["ReferenceFrame", str], visited: List[str] = None) -> np.ndarray:
        """
        Transforms the provided position from the provided src_frame to this frame, if such a transformation is defined
        by the relationships between self, frame and any intermediate frames.
        
        A position is a vector that represents a translation from an origin in relation to a reference axes.
        """
        return src_frame.get_transformation_to(self).transform_coordinate(pos)

    # def transform_vector_from_frame(self, vector: np.ndarray, src_frame: Union["ReferenceFrame", str], visited: List[str] = None) -> np.ndarray:
    #     """Transforms the provided vector from the provided src_frame to this frame, if such a transformation is defined
    #     by the relationships between self, frame and any intermediate frames.
    #     """

    #     if not isinstance(src_frame, ReferenceFrame):
    #         src_frame = self._universe.get_frame(src_frame)

    #     # To find a path between the two frames, we perform a depth-first search of the frame graph,
    #     # starting from this frame and ending at src_frame (if such a path exists).
    #     # As the recursion unwinds, we apply each transformation to the vector until we get back to this frame.

    #     # Base case of the recursion, we've reached src_frame
    #     if self.name == src_frame.name:
    #         return vector

    #     # Since the frame graph is cyclic, keep track of frames we've already considered
    #     if visited is None:
    #         visited = []
    #     visited.append(self.name)

    #     # Iterate over the frames related to this frame
    #     for frame_name in self.related_frames:
    #         # Skip frames we've already seen
    #         if frame_name in visited:
    #             continue

    #         # Try this path
    #         frame = self._universe.get_frame(frame_name)
    #         transformed = frame.transform_vector_from_frame(vector, src_frame, visited)

    #         # Discard paths that did not lead to src_frame
    #         if transformed is None:
    #             continue

    #         # If we made it here, it means we found src_frame in this path!
    #         # Start transforming the vector back from src_frame.
    #         return frame.get_transformation_to(to=self).rotate_vector(transformed)

    #     # If we made it here it means none of the unvisited paths starting from this frame lead to src_frame
    #     return None

    def __repr__(self) -> str:
        return self.name

    def __str__(self) -> str:
        return self.name
