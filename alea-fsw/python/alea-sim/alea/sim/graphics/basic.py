from abc import ABC, abstractmethod
from typing import TypeVar, Generic, Iterable

import numpy as np

from mpl_toolkits import mplot3d
from mpl_toolkits.mplot3d import art3d
import matplotlib.pyplot as plt

from alea.sim.math_lib import math as adcs_math
from alea.sim.math_lib import shapes

X = TypeVar("X", plt.Axes, mplot3d.Axes3D)
L = TypeVar("L", plt.Line2D, art3d.Line3D)

class Cube:
    RECT =   np.array([
                [-1, -1, 1],
                [-1,  1, 1],
                [ 1,  1, 1],
                [ 1, -1, 1]
            ])

    FACE_COORDS = [
        RECT,                       # z+
        -RECT,                      # z-
        np.roll(RECT, 1, axis=1),   # x+
        -np.roll(RECT, 1, axis=1),  # x-
        np.roll(RECT, -1, axis=1),  # y+
        -np.roll(RECT, -1, axis=1), # y-
    ]

    TRANSPARENCY = 0.9
    EDGE_COLOR   = "black"

    FACE_COLORS = [
        "blue",       # z+
        "lightblue",  # z-
        "red",        # x+
        "pink",       # x-
        "green",      # y+
        "lightgreen", # y-
    ]

    def __init__(self, size: float = 1.0):
        self._size = size
        self._rotation = None
        self._faces: list[art3d.Poly3DCollection] = []

    def plot(self, ax: mplot3d.Axes3D) -> list[plt.Artist]:
        for i in range(6):
            face = self._create_face((Cube.FACE_COORDS[i] * self._size), Cube.FACE_COLORS[i], alpha=Cube.TRANSPARENCY)
            ax.add_collection3d(face)
            self._faces.append(face)

        return self._faces

    def update(self, rotation: adcs_math.Quaternion = None) -> list[plt.Artist]:
        if rotation is None:
            dcm = np.eye(3)
        else:
            dcm = rotation.conjugate.to_DCM()

        for i, coords in enumerate(Cube.FACE_COORDS):
            coords = (coords * self._size) @ dcm
            face = self._faces[i]
            face.set_verts([coords])

            # When saving an animation (rather than showing it) it seems that face.axes.M (which do_3d_projection depends on)
            # is set to None, but the rendering works regardless - we just can't call do_3d_projection
            if face.axes.M is not None:
                face.do_3d_projection()

        return self._faces

    def _create_face(self, coords: np.ndarray, color: str, alpha: float) -> art3d.Poly3DCollection:
        face = art3d.Poly3DCollection([coords], alpha=alpha)
        face.set_facecolor(color)
        face.set_edgecolor(Cube.EDGE_COLOR)
        return face

class Spheroid:
    def __init__(self, geometry: shapes.Spheroid, color: str = "lightgrey", wireframe: bool = True,
                 phi_step_deg: int = 10, theta_step_deg: int = 10, animated: bool = False, **kwargs):
        """Initialize a Spheroid.

        Args:
            geometry:
                Spheroid shape to plot.
            color (optional):
                Color for the wire mesh lines when plotted. Defaults to "blue".
            wireframe (optional):
                If True, only a wireframe will be plotted. Otherwise the filled in surface
                will be plotted. Defaults to True.
            phi_step_deg (optional):
                Step size for azimuthal angle when plotting wireframe in degrees.
                Defaults to 10.
            theta_step_deg (optional):
                Step size for polar angle when plotting wireframe in degrees.
                Defaults to 10.
            animated (optional):
                Set the True if the graphic will be animated. Defaults to False.
            kwargs (optional):
                Additional arguments that will be passed to the matplotlib plot functions.
        """
        self._geometry = geometry
        self._color = color
        self._wireframe = wireframe

        self._phi_step_deg = phi_step_deg
        self._theta_step_deg = theta_step_deg

        self._animated = animated

        self.kwargs = kwargs

        self._artist: art3d.Poly3DCollection | art3d.Line3DCollection = None

    @property
    def geometry(self) -> shapes.Spheroid:
        return self._geometry

    def plot(self, ax: mplot3d.Axes3D) -> list[plt.Artist]:
        x, y, z = self.geometry.cartesian_surface_grid(self._phi_step_deg, self._theta_step_deg)
        if self._wireframe:
            self._artist = ax.plot_wireframe(x, y, z, rstride=1, cstride=1, color=self._color, animated=self._animated, **self.kwargs)
        else:
            self._artist = ax.plot_surface(x, y, z, rstride=1, cstride=1, color=self._color, animated=self._animated, **self.kwargs)

        return [self._artist]

class Sphere(Spheroid):
    def __init__(self, r: float = 1.0, color: str = "lightgrey", wireframe: bool = True, phi_step_deg: int = 10, theta_step_deg: int = 10, animated: bool = False, **kwargs):
        super().__init__(shapes.Sphere(r), color=color, wireframe=wireframe, phi_step_deg=phi_step_deg, theta_step_deg=theta_step_deg, animated=animated, **kwargs)

class Path(ABC, Generic[X, L]):
    def __init__(self, init_point: np.ndarray, color: str, alpha: float = 1.0, skip_first: bool = True, max_points: int = 0, clear_on_jump: int = None, animated: bool = False, **kwargs):
        self._init_point = init_point
        self._color = color
        self._alpha = alpha
        self._max_points = max_points
        self._clear_on_jump = clear_on_jump
        self._animated = animated
        self._kwargs = kwargs

        self._points = []

        if not skip_first:
            self._points.append(init_point)

        self._line: L = None

    def calc_coords(self) -> np.ndarray:
        return np.array(self._points).T

    def plot(self, ax: X) -> list[plt.Artist]:
        self._line, = ax.plot(*(self._init_point), color=self._color, alpha=self._alpha, animated=self._animated, **self._kwargs)
        return [self._line]

    def update(self, new_point: np.ndarray) -> list[plt.Artist]:
        # Check if the new_point has a jump discontinuity from the previous point at least as large as self._clear_on_jump.
        # If it does, then we clear the path history
        if (self._clear_on_jump is not None) and (len(self._points) > 0):
            if np.max(np.abs(new_point - self._points[-1])) >= self._clear_on_jump:
                self.clear()

        self._points.append(new_point)
        if (self._max_points > 0) and (len(self._points) > self._max_points):
            self._points = self._points[1:]

        coords = self.calc_coords()
        self.set_data(self._line, coords)
        return [self._line]

    def clear(self):
        self._points.clear()

    @staticmethod
    @abstractmethod
    def set_data(line: L, coords: Iterable):
        raise NotImplementedError

class Path2D(Path[plt.Axes, plt.Line2D]):
    @staticmethod
    def set_data(line: plt.Line2D, coords: Iterable):
        line.set_data(*coords)

class Path3D(Path[mplot3d.Axes3D, art3d.Line3D]):
    @staticmethod
    def set_data(line: art3d.Line3D, coords: Iterable):
        line.set_data_3d(*coords)

class Vector(Generic[X, L]):
    PATH_CLS: type[Path[X, L]] = None

    def __init__(self, value: np.ndarray, color: str, alpha: float = 1.0, draw_line: bool = True, marker_symbol: str = "o", label: str = None,
                 normalize: bool = False, trace_path: bool = False, max_trace_points: int = 10, clear_on_jump: int = None, animated: bool = False, **kwargs):
        self._value = value
        self._color = color
        self._alpha = alpha
        self._draw_line = draw_line
        self._marker_symbol = marker_symbol
        self._label = label
        self._normalize = normalize
        self._trace_path = trace_path
        self._animated = animated
        self._kwargs = kwargs

        if self._trace_path:
            self._path: Path[X, L] = self.PATH_CLS(self._value, self._color, alpha=self._alpha, max_points=max_trace_points, skip_first=True, clear_on_jump=clear_on_jump, animated=animated, **kwargs)

        self._line: L = None
        self._marker: L = None

    def calc_coords(self) -> np.ndarray:
        value = self._value
        if self._normalize:
            norm = np.linalg.norm(value)
            if norm != 0.0:
                value /= norm

        coords = np.array(list(map(lambda x : [0, x], value)))
        return coords

    def clear_path(self):
        self._path.clear()

    def plot(self, ax: X) -> list[plt.Artist]:
        artists = []

        coords = self.calc_coords()

        # Plot trace
        if self._trace_path:
            trace_artists = self._path.plot(ax)
            artists.extend(trace_artists)

        # Plot line
        if self._draw_line:
            self._line, = ax.plot(*coords, color=self._color, alpha=self._alpha, label=self._label, animated=self._animated, **self._kwargs)
            artists.append(self._line)

        # Plot marker
        if self._marker_symbol is not None:
            end_coords = list(map(lambda x : x[1], coords))
            self._marker, = ax.plot(*end_coords, marker=self._marker_symbol, color=self._color, alpha=self._alpha, animated=self._animated, **self._kwargs)
            artists.append(self._marker)

        return artists

    def update(self, value: np.ndarray) -> list[plt.Artist]:
        artists = []

        self._value = value
        coords = self.calc_coords()

        # Update trace
        if self._trace_path:
            trace_artists = self._path.update(self._value)
            artists.extend(trace_artists)

        # Update line
        if self._draw_line:
            self.PATH_CLS.set_data(self._line, coords)
            artists.append(self._line)

        # Update marker
        if self._marker_symbol is not None:
            end_coords = list(map(lambda x : [x[1]], coords))
            self.PATH_CLS.set_data(self._marker, end_coords)
            artists.append(self._marker)

        return artists

class Vector2D(Vector[plt.Axes, plt.Line2D]):
    PATH_CLS = Path2D

class Vector3D(Vector[mplot3d.Axes3D, art3d.Line3D]):
    PATH_CLS = Path3D

class CoordinateAxis:
    def __init__(self, axis: int, length: float, color: str, alpha: float = 1.0, marker_symbol: str = ">", label: str = None,
                 include_neg: bool = False, trace_path: bool = False, max_trace_points: int = 10, animated: bool = False, **kwargs):
        self._axis = axis
        self._length = length
        self._color = color
        self._alpha = alpha
        self._marker_symbol = marker_symbol
        self._include_neg = include_neg

        self._rotation: adcs_math.Quaternion = None

        pos_endpoint = self.calc_pos_endpoint()
        self._pos_axis = Vector(pos_endpoint, color, alpha=alpha, marker_symbol=marker_symbol, label=label,
                                trace_path=trace_path, max_trace_points=max_trace_points, animated=animated, **kwargs)

        if self._include_neg:
            neg_endpoint = self.calc_neg_endpoint()
            self._neg_axis = Vector(neg_endpoint, color, alpha=alpha, marker_symbol=None, animated=animated, **kwargs)

    def calc_pos_endpoint(self) -> np.ndarray:
        return self._endpoint(self._length)

    def calc_neg_endpoint(self) -> np.ndarray:
        return self._endpoint(-self._length)

    def _endpoint(self, length: int) -> np.ndarray:
        coords = np.array([0, 0, 0])
        coords[self._axis] = length

        if self._rotation is not None:
            dcm = self._rotation.to_DCM()
            coords = dcm @ coords

        return coords

    def plot(self, ax: mplot3d.Axes3D) -> list[plt.Artist]:
        artists = []

        artists.extend(self._pos_axis.plot(ax))

        if self._include_neg:
            artists.extend(self._neg_axis.plot(ax))

        return artists

    def update(self, rotation: adcs_math.Quaternion) -> list[plt.Artist]:
        artists = []

        self._rotation = rotation

        pos_endpoint = self.calc_pos_endpoint()
        artists.extend(self._pos_axis.update(pos_endpoint))

        if self._include_neg:
            neg_endpoint = self.calc_neg_endpoint()
            artists.extend(self._neg_axis.update(neg_endpoint))

        return artists

class CoordinateAxes:
    def __init__(self, length: float, colors: list[str], alpha: float = 1.0, marker_symbol: str = ">", labels: list[str] = None,
                 include_neg: bool = False, trace_path: bool = False, max_trace_points: int = 10, draw_origin: bool = False, axes: list[int] = None,
                 animated: bool = False, **kwargs):
        self._draw_origin = draw_origin

        self._axes: list[CoordinateAxis] = []

        if axes is None:
            axes = [0, 1, 2]

        for i, axis in enumerate(axes):
            label = labels[i] if labels is not None else None
            self._axes.append(CoordinateAxis(axis, length, colors[i], alpha=alpha, marker_symbol=marker_symbol, label=label,
                                             include_neg=include_neg, trace_path=trace_path, max_trace_points=max_trace_points,
                                             animated=animated, **kwargs))

    def plot(self, ax: mplot3d.Axes3D) -> list[plt.Artist]:
        if self._draw_origin:
            self._origin = ax.plot(0, 0, 0, color="black", marker="o")

        artists = []

        for axis in self._axes:
            axis_artists = axis.plot(ax)
            artists.extend(axis_artists)

        return artists

    def update(self, rotation: adcs_math.Quaternion) -> list[plt.Artist]:
        artists = []

        for axis in self._axes:
            axis_artists = axis.update(rotation)
            artists.extend(axis_artists)

        return artists
