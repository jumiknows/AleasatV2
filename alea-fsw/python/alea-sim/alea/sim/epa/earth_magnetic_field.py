from typing import TypeAlias, Generator, Iterator
from dataclasses import dataclass
from pathlib import Path
from math import degrees

import numpy as np

import skyfield.timelib

from pygeomag import GeoMag, GeoMagResult

from alea.sim.configuration import Configurable
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.kernel.time_cached import TimeCachedModel, time_cached_property
from alea.sim.kernel.kernel import AleasimKernel, SharedMemoryModelInterface
from alea.sim.kernel.scheduler import EventPriority
from alea.sim.compute.precomputed_bg import PreComputedBG
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel, OrbitDynamicsData
from alea.sim.utils import skyfield_utils

EarthMagneticFieldPreComputeDataPoint: TypeAlias = tuple[skyfield.timelib.Time, np.ndarray]

class EarthMagneticFieldComputeFunc:
    def __init__(self):
        self._wmm = GeoMag()

    def __call__(self, geo_position: tuple[skyfield.timelib.Time, np.ndarray]) -> EarthMagneticFieldPreComputeDataPoint:
        t, position_lla = geo_position
        year_fraction = t.J

        if year_fraction < 2020.0 or year_fraction > 2025.0:
            raise ValueError(f"year fraction {year_fraction} is out of the valid range for the WMM2020 model 2020.0 - 2025.0")

        latitude_deg:  float = degrees(position_lla[0])
        longitude_deg: float = degrees(position_lla[1])
        altitude_km:   float = position_lla[2] / 1000

        result: GeoMagResult = self._wmm.calculate(latitude_deg, longitude_deg, altitude_km, year_fraction, raise_in_warning_zone=True)

        mag_field_vec_ned = np.array([result.x, result.y, result.z])
        return (t, mag_field_vec_ned)

class EarthMagneticFieldPreComputeArgGen:
    def __init__(self, odyn_iter: Iterator[OrbitDynamicsData]):
        self._odyn_iter = odyn_iter

    def __iter__(self) -> Generator[tuple[skyfield.timelib.Time, np.ndarray], None, None]:
        for odyn_data in self._odyn_iter:
            # Make sure this data is for a single point in time
            assert(len(odyn_data) == 1)

            yield (odyn_data.t, odyn_data.geo_positions)

@dataclass
class EarthMagneticFieldConfig:
    use_precompute        : bool = False
    update_freq_divider   : int  = 1 # By default, magnetic field vector updates happen at the same frequency as orbit dynamics updates
                                     # If this is set to a number > 1, then magnetic field vector updates will happen at the divided frequency

class EarthMagneticFieldModel(Configurable[EarthMagneticFieldConfig], TimeCachedModel, AbstractModel, SharedMemoryModelInterface):
    def __init__(self, sim_kernel: AleasimKernel, name: str = "earth_magnetic_field", cfg: str | Path | dict | EarthMagneticFieldConfig = "earth_magnetic_field") -> None:
        super().__init__(name=name, sim_kernel=sim_kernel, cfg=cfg, cfg_cls=EarthMagneticFieldConfig)

        # This will be a proxy for self.kernel.skyfield_time that is only updated when this model
        # receives an update event so it is kept in sync with the models variables
        self._skyfield_time = self.kernel.skyfield_time

        self.configure()

    @property
    def skyfield_time(self) -> skyfield.timelib.Time:
        return self._skyfield_time

    # ==============================================================================
    # Configuration
    # ==============================================================================

    def configure(self):
        self._compute_func = EarthMagneticFieldComputeFunc()

        if self.cfg.use_precompute:
            self._pre_computed: PreComputedBG[skyfield.timelib.Time, np.ndarray, np.ndarray] = PreComputedBG(
                self._compute_func,
                buffer_size   = 10000,
                batch         = False,
            )

    @property
    def update_period(self) -> float:
        return self._odyn.update_period * self.cfg.update_freq_divider # a frequency divider is a period multiplier

    # ==============================================================================
    # Kernel Events
    # ==============================================================================

    def connect(self):
        self._odyn: OrbitDynamicsModel = self.kernel.get_model(OrbitDynamicsModel)

        if self.cfg.use_precompute:
            if not self._odyn.cfg.use_precompute:
                raise ValueError(f"Cannot pre-compute earth magnetic field model if orbit dynamics is not pre-computed")

            # Create argument generator
            odyn_iter = self._odyn.subscribe_to_pre_computed()
            self._pre_computed_arg_gen = EarthMagneticFieldPreComputeArgGen(odyn_iter)

            # Create iterator for ourselves
            self._pre_computed_iter = iter(self._pre_computed.subscribe())

    def start(self):
        if self.cfg.use_precompute:
            self._pre_computed.start(self._pre_computed_arg_gen)

        self.kernel.schedule_event(0, EventPriority.EARTH_MAGNETIC_FIELD_EVENT, self.update, period=self.update_period)

    # ==============================================================================
    # Simulation Variables
    # ==============================================================================

    # By default, these are calculated only when called (and cached until the next time step)
    # They can also be calculated in the update function if desired

    # Magnetic Field Vector in NED Frame

    @time_cached_property
    def _mag_field_vector_ned(self) -> np.ndarray:
        vector = self._compute_func((self.skyfield_time, self._odyn.position_lla))
        return vector

    @property
    def mag_field_vector_ned(self) -> np.ndarray:
        return self._mag_field_vector_ned

    # Magnetic Field Vector in BODY Frame

    @time_cached_property
    def _mag_field_vector_body(self) -> np.ndarray:
        return self.kernel.body_frame.transform_vector_from_frame(self.mag_field_vector_ned, self.kernel.ned_frame)

    @property
    def mag_field_vector_body(self) -> np.ndarray:
        return self._mag_field_vector_body

    # Magnetic Field Vector in ECI Frame

    @time_cached_property
    def _mag_field_vector_eci(self) -> np.ndarray:
        return self.kernel.eci_frame.transform_vector_from_frame(self.mag_field_vector_ned, self.kernel.ned_frame)

    @property
    def mag_field_vector_eci(self) -> np.ndarray:
        return self._mag_field_vector_eci

    # ==============================================================================
    # State Update
    # ==============================================================================

    def update(self):
        self.invalidate_cache(self.kernel.time_n)
        self._skyfield_time = self.kernel.skyfield_time

        if self.cfg.use_precompute:
            t, self._mag_field_vector_ned = next(self._pre_computed_iter)

            skyfield_utils.assert_time_match("Pre-Computed Time", t,
                                             "Actual Simulation Time", self.skyfield_time,
                                             self.update_period)
        else:
            self._mag_field_vector_ned = self._compute_func((self.skyfield_time, self._odyn.position_lla))

        self._save_state()

    # ==============================================================================
    # Saved State
    # ==============================================================================

    @property
    def saved_state_size(self) -> int:
        return 3

    @property
    def saved_state_element_names(self) -> list[str]:
        return [
            "N", "E", "D"
        ]

    def _save_state(self):
        saved_state = np.zeros(self.saved_state_size, dtype=np.float64)
        saved_state[0:3] = self._mag_field_vector_ned
        self.save_chunk_to_memory(saved_state)
