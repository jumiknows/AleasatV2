import numpy as np

from mpl_toolkits import mplot3d
from mpl_toolkits.mplot3d import art3d
from mpl_toolkits.basemap import Basemap
import matplotlib.pyplot as plt

from alea.sim.math_lib import shapes

from .basic import Spheroid

class Earth2D:
    def __init__(self, plot_equator: bool = True, plot_prime_meridian: bool = False, use_blue_marble: bool = True):
        self._plot_equator = plot_equator
        self._plot_prime_meridian = plot_prime_meridian
        self._use_blue_marble = use_blue_marble

        self._map = Basemap(projection="cyl", resolution="i")

    @property
    def map(self) -> Basemap:
        return self._map

    def plot(self, ax: plt.Axes) -> list[plt.Artist]:
        artists = []

        if self._use_blue_marble:
            bluemarble = self._map.bluemarble(ax=ax)
            artists.append(bluemarble)
        else:
            coastlines = self._map.drawcoastlines(ax=ax)
            artists.append(coastlines)

        if self._plot_equator:
            equator = self._map.drawparallels([0], "red", ax=ax)[0][0][0]
            artists.append(equator)

        if self._plot_prime_meridian:
            prime_meridian = self._map.drawmeridians([0], "red", ax=ax)[0][0][0]
            artists.append(prime_meridian)

        return artists

class Earth3D:
    def __init__(self, scale: float = 1.0, base_color: str = "lightskyblue",
                 plot_grid: bool = True, plot_equator: bool = True, plot_prime_meridian: bool = False, animated: bool = False):
        self._geometry = shapes.WGS84Earth(scale)
        self._base = Spheroid(self._geometry, color=base_color, wireframe=False, phi_step_deg=5, theta_step_deg=5, zorder=0, animated=animated)

        self._plot_grid = plot_grid
        if plot_grid:
            self._grid = Spheroid(self._geometry, color="lightgrey", wireframe=True, phi_step_deg=5, theta_step_deg=5, zorder=1, linewidth=0.1, animated=animated)

        self._plot_equator = plot_equator
        self._plot_prime_meridian = plot_prime_meridian

        self._animated = animated

        self._equator = None
        self._prime_meridian = None

    @property
    def geometry(self) -> shapes.Spheroid:
        return self._geometry

    def plot(self, ax: mplot3d.Axes3D) -> list[art3d.artist.Artist]:
        artists = []

        # Plot base spheroid
        base_artists = self._base.plot(ax)
        artists.extend(base_artists)

        # Plot lat/lon grid
        if self._plot_grid:
            grid_artists = self._grid.plot(ax)
            artists.extend(grid_artists)

        # Plot equator
        if self._plot_equator:
            x, y, z = self._geometry.cartesian_surface_arc_az(theta_deg=90, num_points=100)
            self._equator, = ax.plot(x, y, z, color="red", linewidth=0.75, zorder=2, animated=self._animated)
            artists.append(self._equator)

        # Plot prime meridian
        if self._plot_prime_meridian:
            x, y, z = self._geometry.cartesian_surface_arc_polar(phi_deg=0, num_points=100)
            self._prime_meridian, = ax.plot(x, y, z, color="red", linewidth=0.75, zorder=2, animated=self._animated)
            artists.append(self._prime_meridian)

        return artists
