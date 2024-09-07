from abc import ABC, abstractmethod
from typing import TypeVar, Generic, Any

import numpy as np

from mpl_toolkits import mplot3d
from mpl_toolkits.mplot3d import art3d
import matplotlib.pyplot as plt

from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim import graphics as gfx

from .base_scene import BaseScene, BaseScene2D, BaseScene3D
from .model_data_scene import ModelDataScene

X = TypeVar("X", plt.Axes, mplot3d.Axes3D)
A = TypeVar("A", plt.Artist, art3d.artist.Artist)

class OrbitScene(BaseScene[X, A], ABC, Generic[X, A]):
    VECTOR_CLS: type[gfx.basic.Vector] = None

    def __init__(self, ax: X, odyn: OrbitDynamicsModel, orbit_trail_minutes: float = 30, **kwargs):
        super().__init__(ax, **kwargs)

        self._odyn = odyn

        orbit_trail_points = int((orbit_trail_minutes * 60) / self._odyn.update_period)
        position = self.get_satellite_position()
        self._satellite = self.VECTOR_CLS(position, "magenta", draw_line=False, trace_path=True, max_trace_points=orbit_trail_points)

    def plot(self) -> list[A]:
        artists = []
        artists.extend(self._satellite.plot(self.ax))
        return artists

    def update(self) -> list[A]:
        artists = []

        position = self.get_satellite_position()
        artists.extend(self._satellite.update(position))

        return artists

    @abstractmethod
    def get_satellite_position(self) -> np.ndarray:
        raise NotImplementedError

    @classmethod
    def create(cls: type["OrbitScene"], fig: plt.Figure, subplot_spec: plt.SubplotSpec,
               odyn: OrbitDynamicsModel, orbit_trail_minutes: float = 30, **kwargs) -> "OrbitScene":
        return super().create(fig, subplot_spec, odyn=odyn, orbit_trail_minutes=orbit_trail_minutes, **kwargs)

class OrbitScene2D(OrbitScene[plt.Axes, plt.Artist], BaseScene2D):
    VECTOR_CLS = gfx.basic.Vector2D

    def __init__(self, ax: plt.Axes, odyn: OrbitDynamicsModel, orbit_trail_minutes: float = 30, use_blue_marble: bool = True, **kwargs):
        self._earth = gfx.earth.Earth2D(plot_prime_meridian=True, use_blue_marble=use_blue_marble)
        super().__init__(ax, odyn, orbit_trail_minutes, **kwargs)

    def plot(self) -> list[plt.Artist]:
        artists = super().plot()
        artists.extend(self._earth.plot(self.ax))
        return artists

    def get_satellite_position(self) -> np.ndarray:
        position_lla = self._odyn.position_lla
        lat_deg = np.rad2deg(position_lla[0])
        lon_deg = np.rad2deg(position_lla[1])
        x, y = self._earth.map(lon_deg, lat_deg)
        return np.array([x, y])

    @classmethod
    def create(cls: type["OrbitScene"], fig: plt.Figure, subplot_spec: plt.SubplotSpec,
               odyn: OrbitDynamicsModel, orbit_trail_minutes: float = 30, use_blue_marble: bool = True, **kwargs) -> "OrbitScene":
        return super().create(fig, subplot_spec, odyn=odyn, orbit_trail_minutes=orbit_trail_minutes, use_blue_marble=use_blue_marble, **kwargs)

class OrbitScene3D(OrbitScene[mplot3d.Axes3D, art3d.artist.Artist], BaseScene3D):
    VECTOR_CLS = gfx.basic.Vector3D

    def __init__(self, ax: mplot3d.Axes3D, odyn: OrbitDynamicsModel, orbit_trail_minutes: float = 30, **kwargs):
        super().__init__(ax, odyn, orbit_trail_minutes, **kwargs)

        self._earth = gfx.earth.Earth3D(plot_grid=False, plot_prime_meridian=True)
        size = self._earth.geometry.semi_major_axis

        self._coord_axes = gfx.basic.CoordinateAxes(size * 1.5, ["red", "green", "blue"], alpha=0.25, include_neg=True)

        self.configure_view(size)

    def plot(self) -> list[plt.Artist]:
        artists = super().plot()
        artists.extend(self._earth.plot(self.ax))
        artists.extend(self._coord_axes.plot(self.ax))
        return artists

    def get_satellite_position(self) -> np.ndarray:
        return self._odyn.position_ecef

class AltitudeScene(ModelDataScene[OrbitDynamicsModel]):
    def get_data(self) -> np.ndarray:
        _, _, altitude_m = self.model.position_lla
        return np.array([[self.model.kernel.time], [altitude_m / 1000]])

    @classmethod
    def create(cls: type["AltitudeScene"], fig: plt.Figure, subplot_spec: plt.SubplotSpec,
               model: OrbitDynamicsModel, xlim: tuple[float, float], ylim: tuple[float, float],
               plot_args: list[dict[str, Any]] = None,
               **kwargs) -> "AltitudeScene":

        return super().create(fig, subplot_spec,
                              model, xlim=xlim, ylim=ylim,
                              plot_args=plot_args,
                              title="Spacecraft Altitude above WGS84 Reference Ellipsoid", xlabel="Time (s)", ylabel="Altitude [km]",
                              **kwargs)
