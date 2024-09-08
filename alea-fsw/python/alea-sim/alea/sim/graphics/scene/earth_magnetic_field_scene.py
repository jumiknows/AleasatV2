from typing import Any

import numpy as np

import matplotlib.pyplot as plt

from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel

from .model_data_scene import ModelDataScene

class EarthMagneticFieldIntensityScene(ModelDataScene[EarthMagneticFieldModel]):
    def get_mag_field_vector(self) -> np.ndarray:
        mag_field = self._model.mag_field_vector_eci
        mag_field_norm = mag_field / np.linalg.norm(mag_field)
        return mag_field_norm

    def get_data(self) -> np.ndarray:
        mag_field = self.model.mag_field_vector_eci
        return np.array([[self.model.kernel.time], [np.linalg.norm(mag_field)]])

    @classmethod
    def create(cls: type["EarthMagneticFieldIntensityScene"], fig: plt.Figure, subplot_spec: plt.SubplotSpec,
               model: EarthMagneticFieldModel, xlim: tuple[float, float], ylim: tuple[float, float],
               plot_args: list[dict[str, Any]] = None,
               **kwargs) -> "EarthMagneticFieldIntensityScene":

        return super().create(fig, subplot_spec,
                              model, xlim=xlim, ylim=ylim,
                              plot_args=plot_args,
                              title="Earth Magnetic Field Intensity", xlabel="Time [s]", ylabel="Magnetic Field Intensity [nT]",
                              **kwargs)
