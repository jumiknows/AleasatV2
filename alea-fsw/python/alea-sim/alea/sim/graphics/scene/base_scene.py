from abc import ABC, abstractmethod
from typing import TypeVar, Generic

from mpl_toolkits import mplot3d
from mpl_toolkits.mplot3d import art3d
import matplotlib.pyplot as plt

X = TypeVar("X")
A = TypeVar("A")

class BaseScene(ABC, Generic[X, A]):
    def __init__(self, ax: X, **kwargs):
        super().__init__(**kwargs)
        self._ax = ax

    @property
    def ax(self) -> X:
        return self._ax

    @abstractmethod
    def plot(self) -> list[A]:
        raise NotImplementedError

    @abstractmethod
    def update(self) -> list[A]:
        raise NotImplementedError

T2 = TypeVar("T2", bound="BaseScene2D")

class BaseScene2D(BaseScene[plt.Axes, plt.Artist]):
    @classmethod
    def create(cls: type[T2], fig: plt.Figure, subplot_spec: plt.SubplotSpec, **kwargs) -> T2:
        if subplot_spec is None:
            ax: plt.Axes = fig.add_subplot()
        else:
            ax: plt.Axes = fig.add_subplot(subplot_spec)

        return cls(ax, **kwargs)

T3 = TypeVar("T3", bound="BaseScene3D")

class BaseScene3D(BaseScene[mplot3d.Axes3D, art3d.artist.Artist]):
    def configure_view(self, size: float):
        self.ax.set_box_aspect((1, 1, 1))
        self.ax.set_axis_off()
        self.ax.view_init(azim=45)

        self.ax.axes.set_xlim3d(left=-size, right=size)
        self.ax.axes.set_ylim3d(bottom=-size, top=size)
        self.ax.axes.set_zlim3d(bottom=-size, top=size)

    @classmethod
    def create(cls: type[T3], fig: plt.Figure, subplot_spec: plt.SubplotSpec, **kwargs) -> T3:
        if subplot_spec is None:
            ax: mplot3d.Axes3D = fig.add_subplot(projection='3d', computed_zorder=False)
        else:
            ax: mplot3d.Axes3D = fig.add_subplot(subplot_spec, projection='3d', computed_zorder=False)

        return cls(ax, **kwargs)
