from abc import ABC, abstractmethod
from typing import TypeVar, Generic, Any

import numpy as np

import matplotlib.pyplot as plt

from .base_scene import BaseScene2D

M = TypeVar("M")

class ModelDataScene(BaseScene2D, ABC, Generic[M]):
    def __init__(self, ax: plt.Axes, model: M,
                 xlim: tuple[float, float], ylim: tuple[float, float],
                 plot_args: list[dict[str, Any]] = None,
                 title: str = None, xlabel: str = None, ylabel: str = None, **kwargs):
        super().__init__(ax, **kwargs)

        self._model = model

        self._xlim = xlim
        self._ylim = ylim

        self._plot_args = plot_args

        self._title = title
        self._xlabel = xlabel
        self._ylabel = ylabel

        self._data: np.ndarray = None
        self._lines: list[plt.Line2D] = []

    @property
    def model(self) -> M:
        return self._model

    @abstractmethod
    def get_data(self) -> np.ndarray:
        raise NotImplementedError

    def plot(self) -> list[plt.Artist]:
        self._data = self.get_data()

        plot_args = self._plot_args
        if plot_args is None:
            plot_args = [{}] * (len(self._data) - 1)

        for series, plot_args in zip(self._data[1:], plot_args):
            line, = self.ax.plot(self._data[0], series, **plot_args)
            self._lines.append(line)

        self.ax.set_xlim(self._xlim)
        self.ax.set_ylim(self._ylim)

        self.ax.grid(which="both")

        if self._title:
            self.ax.set_title(self._title)
        if self._xlabel:
            self.ax.set_xlabel(self._xlabel)
        if self._ylabel:
            self.ax.set_ylabel(self._ylabel)

        return self._lines

    def update(self) -> list[plt.Artist]:
        new_data = self.get_data()
        self._data = np.append(self._data, new_data, axis=1)

        for series, line in zip(self._data[1:], self._lines):
            line.set_data(self._data[0], series)

        return self._lines

    @classmethod
    def create(cls: type["ModelDataScene"], fig: plt.Figure, subplot_spec: plt.SubplotSpec,
               model: M, xlim: tuple[float, float], ylim: tuple[float, float],
               plot_args: list[dict[str, Any]] = None,
               title: str = None, xlabel: str = None, ylabel: str = None,
               **kwargs) -> "ModelDataScene":

        return super().create(fig, subplot_spec,
                              model=model, xlim=xlim, ylim=ylim,
                              plot_args=plot_args,
                              title=title, xlabel=xlabel, ylabel=ylabel,
                              **kwargs)
