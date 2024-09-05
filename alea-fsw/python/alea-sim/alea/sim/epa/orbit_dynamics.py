from typing import Iterable, Iterator, ClassVar
from enum import Enum
import datetime
from datetime import timezone
import dataclasses
from pathlib import Path

import numpy as np

from sgp4.api import Satrec, WGS84

import skyfield
from skyfield.api import EarthSatellite, load, wgs84
import skyfield.vectorlib
import skyfield.jpllib
import skyfield.timelib
import skyfield.positionlib
from skyfield.positionlib import Barycentric, Geocentric, Apparent
from skyfield.elementslib import osculating_elements_of, OsculatingElements
from skyfield.toposlib import GeographicPosition

from alea.sim.configuration import Configurable
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.kernel.time_cached import TimeCachedModel, time_cached_property
from alea.sim.kernel.kernel import AleasimKernel, SharedMemoryModelInterface
from alea.sim.kernel.scheduler import EventPriority
from alea.sim.epa.frame_conversions import eci_to_ecef_velocity
from alea.sim.compute.precomputed_bg import PreComputedBG
from alea.sim.utils import skyfield_utils

@dataclasses.dataclass(frozen=True)
class OrbitDynamicsData:
    """Data point produced by OrbitDynamicsComputeFunc. This represents the orbit dynamics data either at a single time point
    or at a series of time points.

    Instances of this class are iterable (even if the instance represents a single data point in which case the iterator will
    only yield one item and len(instance) will return 1).
    """

    t                : skyfield.timelib.Time
    sat_states       : np.ndarray
    geo_positions    : np.ndarray
    sun_vectors      : np.ndarray
    is_sunlit        : Iterable[bool] | bool
    orbital_elements : np.ndarray

    def __len__(self) -> int:
        return skyfield_utils.time_len(self.t)

    def __iter__(self) -> Iterator["OrbitDynamicsData"]:
        if len(self) > 1:
            # Zip together data at each time point (all of the fields have an extra dimension corresponding to different time points)
            as_tuple = dataclasses.astuple(self)
            yield from map(lambda zipped : OrbitDynamicsData(*zipped), zip(*as_tuple))
        else:
            # Just a single data point - yield ourselves
            yield self

@dataclasses.dataclass
class SGP4InitData:
    """Data required to initialize a skyfield EarthSatellite using SGP4 orbital elements (rather than a TLE)
    """

    EPOCH_REF: ClassVar[datetime.datetime] = datetime.datetime(1949, 12, 31, 0, 0, 0, 0, timezone.utc)

    t: skyfield.timelib.Time

    ecc          : float       # eccentricity
    argp         : float       # argument of perigee (radians)
    incl         : float       # inclination (radians)
    mean_anomaly : float       # mean anomaly (radians)
    mean_motion  : float       # mean motion (radians)
    raan         : float       # right ascension of ascending node (radians)
    bstar        : float = 0.0 # bstar: drag coefficient (1/earth radii) TODO add drag

@dataclasses.dataclass
class OrbitDynamicsComputeFunc:
    """
    Class that contains methods to compute orbital dynamics values including:

        - Satellite state (position and velocity)
        - Geographic position (latitude, longitude, altitude)
        - Sun vector
        - Sunlit state
        - Orbital elements
    """
    ts: skyfield.timelib.Timescale

    # One of the following two fields must be provided to initialize the propagator.
    # If both fields are provided, the sgp4_init_data will take precedence over the TLE.
    tle: list[str] = None
    sgp4_init_data: SGP4InitData = None

    # ------------------------------------------------------------------------------
    # These internal objects are not pickleable so they must be re-initialized in
    # __setstate__

    _satellite: EarthSatellite = None

    _planets_ephemeris: skyfield.jpllib.SpiceKernel = None

    _earth: skyfield.vectorlib.VectorFunction = None
    _sun: skyfield.vectorlib.VectorFunction = None

    # ------------------------------------------------------------------------------

    def __post_init__(self):
        """Initialize a skyfield EarthSatellite instance from either the TLE or SGP4InitData
        provided at object instantiation. Also load DE421 and initialize ephemeris objects
        for the Earth and Sun.
        """
        if self.sgp4_init_data is not None:
            # See https://pypi.org/project/sgp4/#providing-your-own-elements

            # sgp4 library uses days since 1949 December 31 00:00 UT for the epoch
            now, leap_second = self.sgp4_init_data.t.utc_datetime_and_leap_second()
            epoch = ((now - SGP4InitData.EPOCH_REF).total_seconds() + leap_second) / 86400.0

            satrec = Satrec()
            satrec.sgp4init(
                WGS84, # gravity model
                'i',   # 'a' = old AFSPC mode, 'i' = improved mode
                5,     # satnum: Satellite number
                epoch,
                self.sgp4_init_data.bstar,
                0.0,   # ndot: ballistic coefficient (radians/minute^2) - ignored by SGP4
                0.0,   # nddot: second derivative of mean motion (radians/minute^3) - ignored by SGP4
                self.sgp4_init_data.ecc,
                self.sgp4_init_data.argp,
                self.sgp4_init_data.incl,
                self.sgp4_init_data.mean_anomaly,
                self.sgp4_init_data.mean_motion,
                self.sgp4_init_data.raan,
            )
            self._satellite = EarthSatellite.from_satrec(satrec, self.ts)
        elif self.tle is not None:
            # Although we could directly instantiate an EarthSatellite by passing the TLE to the constructor
            # it will end up using the WGS72 gravity model by default. To use the WGS84 gravity model instead
            # we have to create the Satrec manually
            satrec = Satrec.twoline2rv(self.tle[0], self.tle[1], WGS84)
            self._satellite = EarthSatellite.from_satrec(satrec, self.ts)
        else:
            raise ValueError(f"Either tle or sgp4_init_data must be provided")

        self._planets_ephemeris = load("de421.bsp")

        self._earth = self._planets_ephemeris["earth"]
        self._sun   = self._planets_ephemeris["sun"]

    def __getstate__(self) -> tuple[skyfield.timelib.Timescale, list[int], SGP4InitData]:
        """Return state values to be pickled."""
        # Omit all the unpickleable skyfield attributes.
        # They will be restored by calling __post_init__() in __setstate__()
        return (self.ts, self.tle, self.sgp4_init_data)

    def __setstate__(self, state: tuple[skyfield.timelib.Timescale, list[int], SGP4InitData]):
        """Restore state from the unpickled state values."""
        self.ts, self.tle, self.sgp4_init_data = state
        # Restore unpickleable skyfield attributes
        self.__post_init__()

    def calc_sat_states(self, t: skyfield.timelib.Time) -> tuple[Geocentric, np.ndarray]:
        sat_states = self._satellite.at(t)

        sat_positions = sat_states.position.m
        sat_velocities = sat_states.velocity.m_per_s

        values = [
             sat_positions[0],  sat_positions[1],  sat_positions[2],
            sat_velocities[0], sat_velocities[1], sat_velocities[2],
        ]
        sat_states_arr = skyfield_utils.values_to_np_array(t, values)

        return (sat_states, sat_states_arr)

    def calc_geo_positions(self, sat_states: Geocentric) -> tuple[GeographicPosition, np.ndarray]:
        geo_positions = wgs84.geographic_position_of(sat_states)
        values = [geo_positions.latitude.radians, geo_positions.longitude.radians, geo_positions.elevation.m]
        geo_positions_arr = skyfield_utils.values_to_np_array(sat_states.t, values)
        return (geo_positions, geo_positions_arr)

    def calc_sun_vectors(self, t: skyfield.timelib.Time, sat_states: Geocentric) -> tuple[Apparent, np.ndarray]:
        earth_states = self._earth.at(t)
        sat_positions_barycentric = Barycentric(
            earth_states.position.au       + sat_states.position.au,
            earth_states.velocity.au_per_d + sat_states.velocity.au_per_d,
            t = t,
        )
        sat_positions_barycentric._ephemeris = self._planets_ephemeris
        sun_vectors = sat_positions_barycentric.observe(self._sun).apparent()

        sun_positions = sun_vectors.position.au
        values = [sun_positions[0], sun_positions[1], sun_positions[2]]
        sun_vectors_arr = skyfield_utils.values_to_np_array(t, values)

        return (sun_vectors, sun_vectors_arr)

    def calc_is_sunlit(self, sat_states: Geocentric) -> Iterable[bool] | bool:
        return sat_states.is_sunlit(self._planets_ephemeris)

    def calc_orbital_elements(self, sat_states: Geocentric) -> tuple[OsculatingElements, np.ndarray]:
        orbital_elements = osculating_elements_of(sat_states)
        values = [
            orbital_elements.argument_of_periapsis.radians,
            orbital_elements.longitude_of_ascending_node.radians,
            orbital_elements.inclination.radians,
            orbital_elements.mean_anomaly.radians,
            orbital_elements.mean_motion_per_day.radians,
            orbital_elements.eccentricity
        ]
        orbital_elements_arr = skyfield_utils.values_to_np_array(sat_states.t, values)
        return (orbital_elements, orbital_elements_arr)

    def __call__(self, t: skyfield.timelib.Time) -> OrbitDynamicsData:
        """Calculate all of the orbital dynamics data (see class description)

        Args:
            t:
                A single time at which to calculate the data or a time series
                in which case the data will be calculated at each time point
                in the series

        Returns:
            An OrbitDynamicsData instance containing the orbit dynamics data
            NOTE: The returned object is always iterable even if the time argument is 
                  a single time point (in that case, the iterable has a length of 1).
        """
        sat_states, sat_states_arr = self.calc_sat_states(t)
        _, geo_positions_arr       = self.calc_geo_positions(sat_states)
        _, sun_vectors_arr         = self.calc_sun_vectors(t, sat_states)
        is_sunlit                  = self.calc_is_sunlit(sat_states)
        _, orbital_elements_arr    = self.calc_orbital_elements(sat_states)

        return OrbitDynamicsData(t, sat_states_arr, geo_positions_arr, sun_vectors_arr, is_sunlit, orbital_elements_arr)

@dataclasses.dataclass
class OrbitDynamicsConfig:
    class Propagator(Enum):
        SGP4 = "sgp4"

    class ParameterType(Enum):
        TLE = "tle"
        ORBIT_ELEMENTS = "orbit_elements"

    propagator            : Propagator    = Propagator.SGP4
    parameter_type        : ParameterType = ParameterType.TLE
    use_precompute        : bool          = False
    precompute_batch_size : int           = 2
    tle                   : list[str]     = None
    orbit_elements        : dict          = None
    update_period         : float         = None # kernel timestep will be used if this is 0 or None

    def __post_init__(self):
        # Coerce enum type parameters - this will automatically throw exceptions if an invalid input was used
        self.propagator     = self.Propagator(self.propagator)
        self.parameter_type = self.ParameterType(self.parameter_type)

        if self.parameter_type == self.ParameterType.TLE:
            assert self.tle is not None
        elif self.parameter_type == self.ParameterType.ORBIT_ELEMENTS:
            assert self.orbit_elements is not None

class OrbitDynamicsModel(Configurable[OrbitDynamicsConfig], TimeCachedModel, SharedMemoryModelInterface, AbstractModel):
    """
    Model for the orbit dynamics of a spacecraft

    The spacecraft orbit state is a 6 element state vector
     - 3 position elements
     - 3 velocity elements
    """

    def __init__(self, sim_kernel: AleasimKernel, name: str = "orbit_dynamics", cfg: str | Path | dict | OrbitDynamicsConfig = "orbit_dynamics",
                 compute_func: OrbitDynamicsComputeFunc = None) -> None:
        super().__init__(name=name, sim_kernel=sim_kernel, cfg=cfg, cfg_cls=OrbitDynamicsConfig)
        self._compute_func = compute_func

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
        if self._compute_func is None:
            if self.cfg.parameter_type == OrbitDynamicsConfig.ParameterType.TLE:
                self._compute_func = OrbitDynamicsComputeFunc(self.kernel.timescale, tle=self.cfg.tle)
            elif self.cfg.parameter_type == OrbitDynamicsConfig.ParameterType.ORBIT_ELEMENTS:
                sgp4_init_data = SGP4InitData(self.skyfield_time, **self.cfg.orbit_elements)
                self._compute_func = OrbitDynamicsComputeFunc(self.kernel.timescale, sgp4_init_data=sgp4_init_data)
            else:
                raise ValueError(f"Unsupported parameter type: {self.cfg.parameter_type}")

        if self.cfg.use_precompute:
            self._pre_computed: PreComputedBG[skyfield.timelib.Time, OrbitDynamicsData, OrbitDynamicsData] = PreComputedBG(
                self._compute_func,
                buffer_size   = 10,
                batch         = True,
            )

    @property
    def update_period(self) -> float:
        return self.cfg.update_period or self.kernel.timestep

    # ==============================================================================
    # Kernel Events
    # ==============================================================================

    def connect(self):
        if self.cfg.use_precompute:
            self._pre_computed_arg_gen = skyfield_utils.SkyfieldTimeGen(self.skyfield_time, self.kernel.timestep, self.cfg.precompute_batch_size, self.kernel.timescale)
            self._pre_computed_iter = iter(self._pre_computed.subscribe())

    def start(self):
        if self.cfg.use_precompute:
            self._pre_computed.start(self._pre_computed_arg_gen)

        self.kernel.schedule_event(0, EventPriority.ORBIT_DYNAMICS_EVENT, self.update, period=self.update_period)

    # ==============================================================================
    # Pre-Computation
    # ==============================================================================

    def subscribe_to_pre_computed(self) -> Iterable[OrbitDynamicsData]:
        if not self.cfg.use_precompute:
            raise RuntimeError(f"Cannot subscribe to PreComputed when use_precompute is False")

        return self._pre_computed.subscribe()

    # ==============================================================================
    # Simulation Variables
    # ==============================================================================

    # By default, these are calculated only when called (and cached until the next time step)
    # They can also be calculated in the update function if desired

    @time_cached_property
    def _state(self) -> np.ndarray:
        sat_state_skyfield, sat_state = self._compute_func.calc_sat_states(self.skyfield_time)
        self._state_skyfield = sat_state_skyfield
        return sat_state

    @time_cached_property
    def _state_skyfield(self) -> Geocentric:
        sat_state_skyfield, sat_state = self._compute_func.calc_sat_states(self.skyfield_time)
        self._state = sat_state
        return sat_state_skyfield

    # Geographic Position (longitude [radians], latitude [radians], altitude [km] referenced to wgs84 ellipsoid)
    @time_cached_property
    def _position_lla(self) -> np.ndarray:
        _, geo_position = self._compute_func.calc_geo_positions(self._state_skyfield)
        return geo_position

    @property
    def position_lla(self) -> np.ndarray:
        return self._position_lla

    # Sun Vector
    @time_cached_property
    def _sun_vector(self) -> np.ndarray:
        _, sun_vector = self._compute_func.calc_sun_vectors(self.skyfield_time, self._state_skyfield)
        return sun_vector

    @property
    def sun_vector(self) -> np.ndarray:
        return self._sun_vector

    # Is Sunlit
    @time_cached_property
    def _is_sunlit(self) -> bool:
        return self._compute_func.calc_is_sunlit(self._state_skyfield)

    @property
    def is_sunlit(self) -> bool:
        return self._is_sunlit

    # Orbital Elements in radians [argument_of_periapsis, longitude_of_ascending_node, inclination, mean_anomaly]
    @time_cached_property
    def _orbital_elements(self) -> np.ndarray:
        _, orbital_elements = self._compute_func.calc_orbital_elements(self._state_skyfield)
        return orbital_elements

    @property
    def orbital_elements(self) -> np.ndarray:
        return self._orbital_elements

    # ==============================================================================
    # Derived Variables
    # ==============================================================================

    # Kinematics in ECI frame

    @property
    def position_eci(self) -> np.ndarray:
        """position [m] in ECI coordinates"""
        return self._state[0:3]

    @property
    def velocity_eci(self) -> np.ndarray:
        """velocity [m/s] in ECI coordinates"""
        return self._state[3:6]

    # Kinematics in ECEF frame

    @time_cached_property
    def position_ecef(self) -> np.ndarray:
        """position [m] in ECEF  coordinates"""
        return self.kernel.ecef_frame.transform_position_from_frame(self._state[0:3], self.kernel.eci_frame)

    @time_cached_property
    def velocity_ecef(self) -> np.ndarray:
        """velocity [m/s] in ECEF coordinates"""
        return eci_to_ecef_velocity(self.position_ecef, self.velocity_eci)

    # Other derived variables

    @time_cached_property
    def sun_vector_norm(self) -> np.ndarray:
        return self.sun_vector / np.linalg.norm(self.sun_vector)

    @time_cached_property
    def translational_kinetic_energy(self) -> float:
        """K = 1/2 * M * v^2 [Joules]"""
        v = self.velocity_eci
        M = self.params.get('M')
        ke = (0.5 * M * (v @ v.T))
        return ke

    # ==============================================================================
    # State Update
    # ==============================================================================

    def update(self):
        self.invalidate_cache(self.kernel.time_n)
        self._skyfield_time = self.kernel.skyfield_time

        if self.cfg.propagator == OrbitDynamicsConfig.Propagator.SGP4:
            if self.cfg.use_precompute:
                # Get next data point from the data iterator and update all the simulation variables
                # (since they've been computed in the background already)
                data_point = next(self._pre_computed_iter)

                # Make sure this data is for a single point in time
                assert(len(data_point) == 1)

                # Verify the timestamp of the data point matches the current simulation timestamp
                skyfield_utils.assert_time_match("Pre-Computed Time", data_point.t,
                                                 "Actual Simulation Time", self.skyfield_time,
                                                 self.update_period)

                # Update simulation variables
                self._state            = data_point.sat_states
                self._position_lla     = data_point.geo_positions
                self._sun_vector       = data_point.sun_vectors
                self._is_sunlit        = data_point.is_sunlit
                self._orbital_elements = data_point.orbital_elements
            else:
                # Compute only the satellite state, allow the other simulation variables to be updated on-the-fly if they're "gotten"
                self._state_skyfield, self._state = self._compute_func.calc_sat_states(self.skyfield_time)
        else:
            raise ValueError(f"Unsupported propagator: {self.cfg.propagator}")

        self._save_state()

    # ==============================================================================
    # Saved State
    # ==============================================================================

    @property
    def saved_state_size(self) -> int:
        return self._state.size + self._position_lla.size + self._orbital_elements.size

    @property
    def saved_state_element_names(self) -> list[str]:
        return [
            "x", "y", "z", "vx", "vy", "vz",
            "lon", "lat", "alt",
            "argp", "raan", "incl", "mean_anomaly", "mean_motion", "eccentricity"
        ]

    def _save_state(self):
        saved_state = np.zeros(self.saved_state_size, dtype=np.float64)
        saved_state[0:6]  = self._state
        saved_state[6:9]  = self._position_lla
        saved_state[9:15] = self._orbital_elements
        self.save_chunk_to_memory(saved_state)
