"""
Handle reference frame transformations, and frame trees

Authors: Noah Tajwar, Yousif El-Wishahy 

ALEASAT 2024
"""

from typing import Type, Union, Dict, List, Set
from dataclasses import dataclass
import weakref
from alea.sim.math_lib import cross

import numpy as np

from alea.sim.math_lib import math as adcs_math
from alea.sim.math_lib import Quaternion

@dataclass
class FrameTransformation:
    """
    A class to represent a homogeneous frame transformation that includes both
    a rotation and an optional translation.

    Attributes:
      rotation (np.ndarray | Quaternion): The rotation component as either a 3x3 DCM or a Quaternion.
        If provided as a quaternion (or a 4-element array), it is converted to a 3x3 DCM.
        So that when called as self.rotation later on, it will always be the 3x3 DCM (rotation matrix).
      translation (np.ndarray): A 3-element translation vector. Defaults to a zero vector.

    The transformation is represented as a 4x4 homogeneous matrix (tf) and its inverse (inv_tf)
    are precomputed during initialization. This class supports operations like rotating vectors,
    translating/transforming coordinates, composing transformations (via __mul__ and @ operators), and obtaining
    the transformation matrix or its inverse.

    References:
      - See "Fundamentals of Spacecraft Attitude Determination and Control" by Markley &
        Crassidis for further details on coordinate transformations.
    """
    rotation: np.ndarray | Quaternion
    translation: np.ndarray = None

    def __post_init__(self):
        # If no translation is provided, assume zero translation.
        if self.translation is None:
            self.translation = np.zeros(3)
        # Convert the rotation to a 3x3 DCM.
        if isinstance(self.rotation, np.ndarray):
            if self.rotation.size == 4:
                # If a 4-element array is provided, treat it as a quaternion.
                self.rotation = Quaternion(self.rotation).to_DCM()
            elif self.rotation.shape != (3, 3):
                raise ValueError('Incorrect rotation shape. Expected a 3x3 matrix or a 4-element quaternion.')
        elif isinstance(self.rotation, Quaternion):
            self.rotation = self.rotation.to_DCM()

        # Precompute the homogeneous transformation matrix and its inverse.
        self.tf = self._create_transformation_matrix()
        self.inv_tf = self._create_transformation_matrix_inverse()

    def _create_transformation_matrix(self) -> np.ndarray:
        """
        Creates a 4x4 homogeneous transformation matrix from the DCM and translation.
        """
        dcm = self.rotation
        tf = np.zeros((4, 4))
        tf[0:3, 0:3] = dcm
        tf[0:3, 3] = self.translation
        tf[3, 3] = 1
        return tf

    def _create_transformation_matrix_inverse(self) -> np.ndarray:
        """
        Computes the inverse of the homogeneous transformation matrix.
        The inverse is given by the transpose of the rotation (for orthonormal matrices)
        and the negative rotated translation.
        """
        inv_rot = self.rotation.T  # DCM inverse is its transpose.
        inv_translation = -1 * (inv_rot @ self.translation)
        inv_tf = np.zeros((4, 4))
        inv_tf[0:3, 0:3] = inv_rot
        inv_tf[0:3, 3] = inv_translation
        inv_tf[3, 3] = 1
        return inv_tf
    
    @property
    def quaternion(self) -> Quaternion:
        """
        Returns a Quaternion representation of the rotation.
        The quaternion is computed from the current DCM if not already cached.
        """
        if not hasattr(self, '_quat'):
            self._quat = Quaternion.from_dcm(self.rotation)
        return self._quat

    @classmethod
    def identity(cls) -> "FrameTransformation":
        """
        Returns the identity transformation (no rotation or translation).
        """
        return FrameTransformation(np.eye(3), np.zeros(3))
    
    @classmethod
    def from_tf_mat(cls, value: np.ndarray) -> "FrameTransformation":
        """
        Creates a FrameTransformation instance from a 4x4 homogeneous transformation matrix.

        Parameters:
          value (np.ndarray): A 4x4 transformation matrix.

        Returns:
          FrameTransformation: The corresponding transformation.
        """
        r = value[0:3, 0:3]
        t = value[0:3, 3]
        return FrameTransformation(r, t)

    @classmethod
    def random(cls, translation_max_magnitude: int = 1000) -> "FrameTransformation":
        """
        Generates a random transformation for testing purposes.

        Parameters:
          translation_max_magnitude (int): Maximum translation magnitude.

        Returns:
          FrameTransformation: A randomly generated transformation.
        """
        random_quat = Quaternion.random()
        random_translation = np.random.random(3) * np.random.randint(1, translation_max_magnitude)
        return FrameTransformation(random_quat, random_translation)

    def rotate_by_axang(self, axis: np.ndarray, angle: float) -> "FrameTransformation":
        """
        Creates a new transformation by applying an additional rotation defined by an axis-angle representation.

        Parameters:
          axis (np.ndarray): The rotation axis (should be a unit vector).
          angle (float): The rotation angle in radians.

        Returns:
          FrameTransformation: The new transformation after applying the rotation.
        """
        q = Quaternion.from_axang(axis, angle)
        return self.rotate_by_q(q)

    def rotate_by_q(self, q: Quaternion) -> "FrameTransformation":
        """
        Creates a new transformation by applying an additional rotation given as a quaternion.

        Parameters:
          q (Quaternion): The quaternion representing the rotation.

        Returns:
          FrameTransformation: The combined transformation.
        """
        next_tf = np.zeros((4, 4))
        next_tf[0:3, 0:3] = q.to_DCM()
        next_tf[3, 3] = 1
        tf_new = self.transformation_matrix() @ next_tf
        return FrameTransformation.from_tf_mat(tf_new)
    
    def inverse(self) -> "FrameTransformation":
        """
        Returns the inverse transformation.

        Returns:
          FrameTransformation: The inverse of the current transformation.
        """
        return self.from_tf_mat(self.inv_transformation_matrix())

    def transformation_matrix(self) -> np.ndarray:
        """
        Returns the homogeneous transformation matrix.

        Returns:
          np.ndarray: The 4x4 transformation matrix.
        """
        return self.tf

    def inv_transformation_matrix(self) -> np.ndarray:
        """
        Returns the inverse of the homogeneous transformation matrix.

        Returns:
          np.ndarray: The 4x4 inverse transformation matrix.
        """
        return self.inv_tf

    def rotate_vector(self, vector: np.ndarray) -> np.ndarray:
        """
        Applies the rotation part of the transformation to a vector.

        Parameters:
          vector (np.ndarray): A 3-element vector.

        Returns:
          np.ndarray: The rotated 3-element vector.
        """
        v_hom = np.zeros(4)
        v_hom[0:3] = vector
        rotated = self.tf @ v_hom
        return rotated[0:3]

    def inverse_rotate_vector(self, vector: np.ndarray) -> np.ndarray:
        """
        Applies the inverse rotation to a vector.

        Parameters:
          vector (np.ndarray): A 3-element vector.

        Returns:
          np.ndarray: The vector after applying the inverse rotation.
        """
        v_hom = np.zeros(4)
        v_hom[0:3] = vector
        rotated = self.inv_tf @ v_hom
        return rotated[0:3]

    def transform_coordinate(self, coordinate: np.ndarray) -> np.ndarray:
        """
        Applies a homogenous transformation to the coordinate where pt = Rotation * p + Translation

        Parameters:
          coordinate (np.ndarray): A 3-element coordinate vector.

        Returns:
          np.ndarray: The transformed coordinate.
        """
        p_hom = np.ones(4)
        p_hom[0:3] = coordinate
        transformed = self.tf @ p_hom
        return transformed[0:3]

    def inverse_transform_coordinate(self, coordinate: np.ndarray) -> np.ndarray:
        """
        Applies the inverse homogeneous transformation to a coordinate.

        Parameters:
          coordinate (np.ndarray): A 3-element coordinate vector.

        Returns:
          np.ndarray: The coordinate in the original frame.
        """
        p_hom = np.ones(4)
        p_hom[0:3] = coordinate
        transformed = self.inv_tf @ p_hom
        return transformed[0:3]

    def __repr__(self) -> str:
        return f'FrameTransformation(tf={self.tf})'

    def __str__(self) -> str:
        return str(self.tf)

    def __mul__(self, tf_rhs: "FrameTransformation") -> "FrameTransformation":
        """
        Composes two frame transformations using multiplication.
        """
        return FrameTransformation.from_tf_mat(self.tf @ tf_rhs.tf)

    def __rmul__(self, tf_lhs: "FrameTransformation") -> "FrameTransformation":
        """
        Right-hand multiplication for composing transformations.
        """
        return FrameTransformation.from_tf_mat(tf_lhs.tf @ self.tf)

    def __matmul__(self, other: "FrameTransformation") -> "FrameTransformation":
        """
        Overloads the @ operator for transformation composition.
        """
        return FrameTransformation.from_tf_mat(self.tf @ other.tf)

    def __rmatmul__(self, other: "FrameTransformation") -> "FrameTransformation":
        """
        Overloads the @ operator for right-hand transformation composition.
        """
        return FrameTransformation.from_tf_mat(other.tf @ self.tf)

class Universe:
    """
    Represents a 'universe' or environment in which multiple ReferenceFrames exist
    and can be related to each other. Each Universe instance tracks the frames
    by name and provides functionality to create new frames or retrieve existing ones.
    """

    def __init__(self, q_class: Type[adcs_math.Quaternion]):
        """Initializes the Universe object.

        Args:
            q_class (Type[adcs_math.Quaternion]): A quaternion class (or similar factory)
                used to generate identity transforms and for quaternion operations.
        """
        self._q_class = q_class

        self._frames: Dict[str, "ReferenceFrame"] = {}

    def get_frame(self, name: str) -> "ReferenceFrame":
        """Retrieves a reference frame by name.

        Args:
            name (str): The name of the frame to retrieve.

        Returns:
            ReferenceFrame: The frame if found, or None if no frame of that name exists.
        """
        try:
            return self._frames[name]
        except KeyError:
            return None

    def create_frame(self, name: str, derived_from: Union["ReferenceFrame", str] = None, transformation: FrameTransformation = None) -> "ReferenceFrame":
        """Creates a new reference frame in this Universe.

        If `derived_from` is specified, the new frame is defined relative to the given frame
        using the provided transformation.

        Args:
            name (str): Name of the new frame.
            derived_from (ReferenceFrame or str, optional): The existing frame (or frame name)
                from which this new frame is derived. Defaults to None.
            transformation (FrameTransformation, optional): The transform mapping
                `derived_from` to the new frame. If None, identity transform is assumed.

        Returns:
            ReferenceFrame: The newly created frame.

        Raises:
            ValueError: If a frame with the given name already exists.
        """
        if name in self._frames:
            raise ValueError(f"Frame with name \"{name}\" already exists")

        new_frame = ReferenceFrame(name, self)
        self._frames[name] = new_frame

        if derived_from is not None:
            new_frame.derive_from(derived_from, transformation)

        return new_frame

    def identity_transform(self) -> FrameTransformation:
        """Returns an identity FrameTransformation.

        The rotation is the identity quaternion (from self._q_class) and the translation
        is a zero vector.

        Returns:
            FrameTransformation: A transformation representing no rotation or translation.
        """
        return FrameTransformation(self._q_class.identity(), np.zeros(3))

class ReferenceFrame:
    """
    Represents a coordinate frame in the Universe. Each frame can be defined
    relative to other frames, forming a graph of transformations. This class
    stores relationships to other frames and provides methods to derive new
    transformations and apply them to vectors or coordinates.
    """
    def __init__(self, name: str, universe: Universe):
        """Initializes a ReferenceFrame with the given name, belonging to a specific Universe.

        Args:
            name (str): The name of this frame.
            universe (Universe): The Universe instance to which this frame belongs.
        """
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
        """Returns the set of names for frames directly related to this frame.
        """
        return self._relations.keys()
    
    def get_transformation_to(self, dest_frame: Union["ReferenceFrame", str], visited: List[str] = None) -> FrameTransformation:
        """Finds the transformation from this frame to another frame (dest_frame).

        Performs a depth-first search of the frame relationships to chain
        together the transformations. If no path is found, returns None.

        Args:
            dest_frame (ReferenceFrame or str): The destination frame or its name.
            visited (List[str], optional): Internal parameter used during recursion to track
                visited frames. Defaults to None.

        Returns:
            FrameTransformation: The transformation from this frame to dest_frame,
            or None if no path exists.
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
        """Defines a direct relationship from the given frame to this frame.

        In other words, applying the provided `transformation` to `frame`
        should yield this frame.

        Args:
            frame (ReferenceFrame or str): The frame (or frame name) from which
                this frame is derived.
            transformation (FrameTransformation, optional): The transform from `frame` to
                this frame. If None, uses an identity transform.
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
        """Applies an additional rotation to this frame, expressed by an axis-angle.

        Args:
            axis (np.ndarray): A 3-element array representing the rotation axis.
            angle (float): The rotation angle in radians.
            relative_to (ReferenceFrame or str): A directly related frame that defines
                how this rotation is interpreted.
        """
        relative_to = self._check_directly_related(relative_to)

        old_transformation: FrameTransformation = relative_to.get_transformation_to(self)
        new_transformation = old_transformation.rotate_by_axang(axis, angle)
        self.derive_from(relative_to, new_transformation)

    def rotate_by_q(self, q: adcs_math.Quaternion, relative_to: Union["ReferenceFrame", str]):
        """Applies an additional rotation to this frame, expressed by a quaternion.

        Args:
            q (adcs_math.Quaternion): The quaternion describing the rotation.
            relative_to (ReferenceFrame or str): A directly related frame that defines
                how this rotation is interpreted.
        """
        relative_to = self._check_directly_related(relative_to)

        old_transformation = relative_to.get_transformation_to(self)
        new_transformation = old_transformation.rotate_by_q(q)
        self.derive_from(relative_to, new_transformation)
    
    def transform_vector_from_frame(self, vector: np.ndarray, src_frame: Union["ReferenceFrame", str]) -> np.ndarray:
        """Transforms the provided vector from the provided src_frame to this frame, if such a transformation is defined
        by the relationships between self, frame and any intermediate frames.

        The vector is assumed to be purely directional (no translation component).

        Args:
            vector (np.ndarray): The 3D vector in src_frame coordinates.
            src_frame (ReferenceFrame or str): The frame from which the vector originates.

        Returns:
            np.ndarray: The same physical vector, now expressed in this frame's coordinates.
        """
        return src_frame.get_transformation_to(self).rotate_vector(vector)

    def transform_position_from_frame(self, pos: np.ndarray, src_frame: Union["ReferenceFrame", str]) -> np.ndarray:
        """
        Transforms the provided position from the provided src_frame to this frame, if such a transformation is defined
        by the relationships between self, frame and any intermediate frames.
        
        A position is a vector that represents a translation from an origin in relation to a reference axes.

        Args:
            pos (np.ndarray): The 3D position in src_frame coordinates.
            src_frame (ReferenceFrame or str): The source frame.

        Returns:
            np.ndarray: The position expressed in this frame's coordinates.
        """
        return src_frame.get_transformation_to(self).transform_coordinate(pos)

    def transform_velocity_from_frame(self, vel_src: np.ndarray, pos_src: np.ndarray, omega_tgt_src: np.ndarray, src_frame: Union["ReferenceFrame", str]) -> np.ndarray:
        """
        Transforms a velocity from the provided src_frame to this frame, if such a transformation is defined
        by the relationships between self, frame and any intermediate frames.
        
        Accounts for the rotation rate between frames, given by `omega_tgt_src`,
        and the position `pos_src` at which the velocity is measured in src_frame.
        
        omega_tgt_src is defined as the angular velocity of the target frame w.r.t the src frame expressed in src frame coordinates (basis).
        To convert velocities, we must acount for the rotation rate of the target frame relative to the src_frame.
        If omega_tgt_src is 0, transform_vector_from_frame can be used instead of this function.

        Args:
            vel_src (np.ndarray): Velocity in src_frame coordinates.
            pos_src (np.ndarray): Position in src_frame coordinates where velocity is measured.
            omega_tgt_src (np.ndarray): Angular velocity of the target frame relative
                to the source frame, expressed in src_frame coordinates.
            src_frame (ReferenceFrame or str): The source frame.

        Returns:
            np.ndarray: The velocity expressed in this frame's coordinates.
        """
        R_src_self = src_frame.get_transformation_to(self).rotation
        return R_src_self @ (vel_src - cross(omega_tgt_src, pos_src))

    def __repr__(self) -> str:
        return self.name

    def __str__(self) -> str:
        return self.name
