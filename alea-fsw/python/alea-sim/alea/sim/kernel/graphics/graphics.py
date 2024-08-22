from typing import List

import numpy as np

from mpl_toolkits import mplot3d
from mpl_toolkits.mplot3d import art3d

from alea.sim.math_lib import math as adcs_math

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
        self._faces: List[art3d.Poly3DCollection] = []

    @property
    def artists(self) -> List[art3d.artist.Artist]:
        return self._faces

    def plot(self, ax: mplot3d.Axes3D) -> List[art3d.artist.Artist]:
        for i in range(6):
            face = self._create_face((Cube.FACE_COORDS[i] * self._size), Cube.FACE_COLORS[i], alpha=Cube.TRANSPARENCY)
            ax.add_collection3d(face)
            self._faces.append(face)

        return self._faces

    def update(self, rotation: adcs_math.Quaternion = None) -> List[art3d.artist.Artist]:
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

class Sphere:
    def __init__(self, r: float, color: str):
        self._r = r
        self._color = color

        self._artist = None

    @property
    def artists(self) -> List[art3d.artist.Artist]:
        return [self._artist]

    def plot(self, ax: mplot3d.Axes3D):
        u, v = np.mgrid[0:2*np.pi:50j, 0:np.pi:25j]
        x = self._r*np.cos(u)*np.sin(v)
        y = self._r*np.sin(u)*np.sin(v)
        z = self._r*np.cos(v)
        self._artist = ax.plot_wireframe(x, y, z, color=self._color, linewidth=0.5)

class Path:
    def __init__(self, init_point: np.ndarray, color: str, alpha: float = 1.0, skip_first: bool = True, max_points: int = 0):
        self._init_point = init_point
        self._color = color
        self._alpha = alpha
        self._max_points = max_points

        self._points = []

        if not skip_first:
            self._points.append(init_point)

        self._line: art3d.Line3D = None

    @property
    def artists(self) -> List[art3d.artist.Artist]:
        return [self._line]

    def calc_coords(self) -> np.ndarray:
        return np.array(self._points).T

    def plot(self, ax: mplot3d.Axes3D) -> List[art3d.artist.Artist]:
        self._line, = ax.plot(self._init_point[0], self._init_point[1], self._init_point[2], color=self._color, alpha=self._alpha)
        return [self._line]

    def update(self, new_point: np.ndarray) -> List[art3d.artist.Artist]:
        self._points.append(new_point)
        if (self._max_points > 0) and (len(self._points) > self._max_points):
            self._points = self._points[1:]

        coords = self.calc_coords()
        self._line.set_data_3d(coords[0], coords[1], coords[2])
        return [self._line]

class Vector:
    def __init__(self, value: np.ndarray, color: str, alpha: float = 1.0, marker_symbol: str = "o", label: str = None, normalize: bool = False, trace_path: bool = False, max_trace_points: int = 10):
        self._value = value
        self._color = color
        self._alpha = alpha
        self._marker_symbol = marker_symbol
        self._label = label
        self._normalize = normalize
        self._trace_path = trace_path

        if self._trace_path:
            self._path = Path(self._value, self._color, alpha=self._alpha, max_points=max_trace_points)

        self._line: art3d.Line3D = None
        self._marker: art3d.Line3D = None

    @property
    def artists(self) -> List[art3d.artist.Artist]:
        all_artists = [self._line, self._marker]
        if self._trace_path:
            all_artists.extend(self._path.artists)
        return all_artists

    def calc_coords(self) -> np.ndarray:
        value = self._value
        if self._normalize:
            norm = np.linalg.norm(value)
            if norm != 0.0:
                value /= norm

        return np.array([[0, value[0]], [0, value[1]], [0, value[2]]])

    def plot(self, ax: mplot3d.Axes3D) -> List[art3d.artist.Artist]:
        artists = []

        coords = self.calc_coords()

        # Plot trace
        if self._trace_path:
            trace_artists = self._path.plot(ax)
            artists.extend(trace_artists)

        # Plot line
        self._line, = ax.plot(coords[0], coords[1], coords[2], color=self._color, alpha=self._alpha, label=self._label)
        artists.append(self._line)

        # Plot marker
        if self._marker_symbol is not None:
            self._marker, = ax.plot(coords[0][1], coords[1][1], coords[2][1], marker=self._marker_symbol, color=self._color, alpha=self._alpha)
            artists.append(self._marker)

        return artists

    def update(self, value: np.ndarray) -> List[art3d.artist.Artist]:
        artists = []

        self._value = value
        coords = self.calc_coords()

        # Update trace
        if self._trace_path:
            trace_artists = self._path.update(self._value)
            artists.extend(trace_artists)

        # Update line
        self._line.set_data_3d(coords[0], coords[1], coords[2])
        artists.append(self._line)

        # Update marker
        if self._marker_symbol is not None:
            self._marker.set_data_3d([coords[0][1]], [coords[1][1]], [coords[2][1]])
            artists.append(self._marker)

        return artists

class CoordinateAxis:
    def __init__(self, axis: int, length: float, color: str, alpha: float = 1.0, marker_symbol: str = ">", label: str = None, include_neg: bool = False, trace_path: bool = False, max_trace_points: int = 10):
        self._axis = axis
        self._length = length
        self._color = color
        self._alpha = alpha
        self._marker_symbol = marker_symbol
        self._include_neg = include_neg

        self._rotation: adcs_math.Quaternion = None

        pos_endpoint = self.calc_pos_endpoint()
        self._pos_axis = Vector(pos_endpoint, color, alpha=alpha, marker_symbol=marker_symbol, label=label, trace_path=trace_path, max_trace_points=max_trace_points)

        if self._include_neg:
            neg_endpoint = self.calc_neg_endpoint()
            self._neg_axis = Vector(neg_endpoint, color, alpha=alpha, marker_symbol=None)

    @property
    def artists(self) -> List[art3d.artist.Artist]:
        all_artists = self._pos_axis.artists
        if self._include_neg:
            all_artists.extend(self._neg_axis.artists)
        return all_artists

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

    def plot(self, ax: mplot3d.Axes3D) -> List[art3d.artist.Artist]:
        artists = []

        artists.extend(self._pos_axis.plot(ax))

        if self._include_neg:
            artists.extend(self._neg_axis.plot(ax))

        return artists

    def update(self, rotation: adcs_math.Quaternion) -> List[art3d.artist.Artist]:
        artists = []

        self._rotation = rotation

        pos_endpoint = self.calc_pos_endpoint()
        artists.extend(self._pos_axis.update(pos_endpoint))

        if self._include_neg:
            neg_endpoint = self.calc_neg_endpoint()
            artists.extend(self._neg_axis.update(neg_endpoint))

        return artists

class CoordinateAxes:
    def __init__(self, length: float, colors: List[str], alpha: float = 1.0, marker_symbol: str = ">", labels: List[str] = None, include_neg: bool = False, trace_path: bool = False, max_trace_points: int = 10, draw_origin: bool = False, axes: List[int] = None):
        self._draw_origin = draw_origin

        self._axes: List[CoordinateAxis] = []

        if axes is None:
            axes = [0, 1, 2]

        for i, axis in enumerate(axes):
            label = labels[i] if labels is not None else None
            self._axes.append(CoordinateAxis(axis, length, colors[i], alpha=alpha, marker_symbol=marker_symbol, label=label, include_neg=include_neg, trace_path=trace_path, max_trace_points=max_trace_points))

    @property
    def artists(self) -> List[art3d.artist.Artist]:
        all_artists = []
        for axis in self._axes:
            all_artists.extend(axis.artists)
        return all_artists

    def plot(self, ax: mplot3d.Axes3D) -> List[art3d.artist.Artist]:
        if self._draw_origin:
            self._origin = ax.plot(0, 0, 0, color="black", marker="o")

        artists = []
        for axis in self._axes:
            axis_artists = axis.plot(ax)
            artists.extend(axis_artists)

        return artists

    def update(self, rotation: adcs_math.Quaternion) -> List[art3d.artist.Artist]:
        artists = []
        for axis in self._axes:
            axis_artists = axis.update(rotation)
            artists.extend(axis_artists)

        return artists
