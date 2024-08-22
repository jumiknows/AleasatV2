
import numpy as np

from alea.sim.kernel.kernel import AleasimKernel, SharedMemoryModelInterface
from alea.sim.kernel.generic.dynamic_model import DynamicModel
from alea.sim.epa.frame_conversions import *
from skyfield.elementslib import osculating_elements_of, OsculatingElements
from alea.sim.kernel.scheduler import EventPriority

from sgp4.api import Satrec, WGS84
from skyfield.api import EarthSatellite, load, wgs84
import datetime
import skyfield.api as sf
from skyfield.positionlib import Barycentric

from .orbit_utils import rv2coe

mu_earth = 0.3986004415e15  # meters^3/s^2
re_earth =  6378136.6  # meters

class OrbitDynamicsModel(DynamicModel, SharedMemoryModelInterface):
    """
    Model for the orbit dynamics of a spacecraft
    
    The spacecraft orbit state is is a 6 element state vector
     - 3 position elements, 3 velocity elements
    """
    
    default_name = 'orbit_dynamics'
    
    def __init__(self, sim_kernel: AleasimKernel) -> None:
        super().__init__(OrbitDynamicsModel.default_name, sim_kernel,'euler')
        self.configure()
        self.logger.info(f'initialized with state {self._state}')
        
    def configure(self):
        cfg = self.get_config()
        
        self._propagator = cfg['propagator']
        if self._propagator != 'sgp4':
            raise Exception("sgp4 is the only propagator available at the moment.")
        self._state = np.zeros(6)
        self._state_derivative = np.zeros(6)

        if cfg['parameter_type'] == 'tle':
            tle = cfg['tle']
            self._sat = EarthSatellite(tle[1], tle[2], tle[0])
            self.logger.info(f'loaded sgp4 orbit parameters from tle {tle}')
            self.logger.warn(f'overriding simulation epoch time to tle epoch {self._sat.epoch}')
            if self.kernel.time > 0:
                raise Exception("OrbitDynamics configured after simulation time was already propagated.")
            else:
                self.kernel._skyfield_epoch_time = self._sat.epoch
                self.kernel._skyfield_time = self._sat.epoch
        elif cfg['parameter_type'] == 'orbital_elements':
            orbit_cfg:dict = cfg['orbit_elements']
            e = orbit_cfg['e']
            argp = orbit_cfg['argp']
            incl = orbit_cfg['incl']
            mean_anomaly = orbit_cfg['mean_anomaly']
            mean_motion = orbit_cfg['mean_motion']
            raan = orbit_cfg['raan']
            
            #OVERRIDE WITH 0 IF THEY DONT EXIST
            bstar = orbit_cfg.get('bstar', 0)
            raan = orbit_cfg.get('ndot', 0)

            if self._propagator == 'sgp4':
                self._init_sgp4_from_kep(e, argp, incl, mean_anomaly, mean_motion, raan)
            else:
                raise ValueError(f'propagator type {self._propagator} is not recognized!')
        else:
            raise ValueError("orbit dynamics cfg parameter_type was not recognized.")

        # skyfield ephimeris
        # DE421 is a JPL ephemeris file (skyfield handles loading it)
        # see https://rhodesmill.org/skyfield/files.html
        #TODO is DE421 up to date?
        self._planets_ephem = load('de421.bsp')
        self._earth_ephimeris = self._planets_ephem['earth']
        self._sun_ephimeris = self._planets_ephem['sun']

        #the last calculated sun vector
        self._last_sun_vec = None
        #the simulation integer time at which this sun vector was calculated
        #the goal here is avoid recalculation of the sun vector
        self._last_sun_vec_aq_t_n = -1.0

    def connect(self):
        self.kernel.schedule_event(0, EventPriority.DYNAMICS_EVENT, self.update, period=self.kernel.timestep)

    @property
    def config_name(self) -> str:
        return self.default_name

    def _init_sgp4_from_kep(self, ecc, argp, incl, mean_anomaly, mean_motion, raan, bstar=0.0, ndot=0.0):
        """
        init sgp4 satellite object from keplerian orbit elements
        @param ecc - eccentricity
        @param - argp, argument of perigee (radians)
        @param - incl inclination (radians)
        @param - mean anomaly (radians)
        @param - mean_motion (radians/minute)
        @param - raan, # RAAN , right ascension of ascending node (radians)
        """
        #get days since 1949 December 31 00:00 U
        #why ? because it is the time param required for sgp4...
        now, leap_second = self.kernel.date_and_leap_second
        epoch = ((now-datetime.datetime(1949,12,31,0,0,0,0,now.tzinfo)).total_seconds() + leap_second)/86400.0
        
        self._satrec = Satrec()
        self._satrec.sgp4init(
            WGS84,           # gravity model
            'i',             # 'a' = old AFSPC mode, 'i' = improved mode
            5,               # satnum: Satellite number
            epoch,       # epoch: days since 1949 December 31 00:00 UT
            bstar,      # bstar: drag coefficient (/earth radii), TODO change this
            ndot, # ndot: ballistic coefficient (radians/minute^2) TODO change this
            0.0,             # nddot: second derivative of mean motion (radians/minute^3)
            ecc,       # ecC: eccentricity
            argp, # argp: argument of perigee (radians)
            incl, # incl: inclination (radians)
            mean_anomaly, # mean anomaly (radians)
            mean_motion, # mean motion (radians/minute)
            raan, # RAAN , right ascension of ascending node (radians)
        )
        self._ts = self.kernel.timescale
        self._sat = EarthSatellite.from_satrec(self._satrec, self._ts)
        self._sat.epoch

        self._spg4_eci_state = self._sat.at(self.kernel.skyfield_time)
        self._state[0:3] = self._spg4_eci_state.position.m
        self._state[3:6] = self._spg4_eci_state.velocity.m_per_s
        
    @property
    def saved_state_size(self) -> int:
        return self._state.size + 7 #seven extra elements for geographic coords and osculating orbit elements

    @property
    def saved_state_element_names(self) -> list[str]:
        return ['x','y','z','vx','vy','vz','lon','lat','alt','argp','raan','incl','mean_anomaly']

    @property
    def logger(self):
        return self._logger
    
    @property
    def is_sunlit(self) -> bool:
        """Return whether the current position in Earth orbit is in sunlight."""
        return self._spg4_eci_state.is_sunlit()
    
    @property
    def position_eci(self) -> np.ndarray:
        """position [m] in ECI coordinates"""
        return self._state[0:3]

    @property
    def position_ecef(self) -> np.ndarray:
        """position [m] in ECEF  coordinates"""
        #TODO compare values
        # return self._spg4_eci_state.itrf_xyz().m
        #for some reason there is an error for this
        return  self.kernel.ecef_frame.transform_position_from_frame(self._state[0:3], self.kernel.eci_frame)
    
    @property
    def position_lonlat(self):
        """satellite longitude [radians], latitude [radians] and height [km] referenced to wgs84 ellipsoid"""
        lla = wgs84.geographic_position_of(self._spg4_eci_state)
        lat = lla.latitude.radians
        lon = lla.longitude.radians
        height = lla.elevation.km
        return np.array([lat, lon, height])
    
    @property
    def velocity_eci(self) -> np.ndarray:
        """velocity [m/s] in ECI coordinates"""
        return self._state[3:6]
    
    @property
    def velocity_ecef(self) -> np.ndarray:
        """velocity [m/s] in ECEF coordinates"""
        return eci_to_ecef_velocity(self.position_ecef, self._state[3:6])

    @property
    def acceleration_eci(self) -> np.ndarray:
        """[m/s^2] ECI coordinates"""
        return self._state_derivative[3:6]

    @property
    def acceleration_ecef(self) -> np.ndarray:
        """[m/s^2] ECEF coordinates"""
        return eci_to_ecef_acceleration(self.position_ecef, self.velocity_ecef, self._state_derivative[3:6])
    
    @property
    def sun_vector_norm(self) -> np.ndarray:
        sun_vec_au = self.calculate_sun_vector()
        return sun_vec_au/np.linalg.norm(sun_vec_au)
    
    def calculate_orbital_elements(self) -> OsculatingElements:
        """
        Returns skyfield OsculatingElements, which represent the keplerian orbital elements defining the satellites orbit with no disturbances.
        Mainly useful for orbital frame calculations.
        
        see https://rhodesmill.org/skyfield/elements.html
        """
        elements = osculating_elements_of(self._spg4_eci_state)     
        return elements
    
    def eci_state_vector_to_ecef(self, x) -> np.ndarray:
        x_ecef = np.zeros(6)
        x_ecef[0:3] = eci_to_ecef(x[0:3], self.kernel.gmst_rad)
        x_ecef[3:6] = eci_to_ecef_velocity(x_ecef[0:3], x[3:6])
        return x_ecef

    # I had a problem with @property decorator in position_lonlat function, therefore I coppied and pasted it to this new file
    def calculate_pos_vector(self) -> np.ndarray:
        lla = wgs84.geographic_position_of(self._spg4_eci_state)
        lat = lla.latitude.radians
        lon = lla.longitude.radians
        height = lla.elevation.km
        return np.array([lat, lon, height])
    
    def calculate_sun_vector(self) -> np.ndarray:
        """
        Get sun position relative to satellite location in intertial frame. Units are [au].
        Wrapper for _calculate_sun_vector_internal which checks if we already calculated the sun vector at the current kernel.time.
        The reason for doing this is because calculating the apparent sun vector is an expensive operation.
        """

        if self._last_sun_vec_aq_t_n == self.kernel._t_n:
            return self._last_sun_vec
        else:
            return self._calculate_sun_vector_internal()
        

    def _calculate_sun_vector_internal(self) -> np.ndarray:
        """
        Get sun position relative to satellite location in intertial frame. Units are [au].
        This function is expensive, avoid calling it too much.
        """
        #skyfield resources I used to write this code.
        # https://rhodesmill.org/skyfield/positions.html#barycentric-astrometric-apparent
        # https://github.com/skyfielders/python-skyfield/discussions/604
        
        t: sf.Time = self.kernel.skyfield_time #simulation represented as skyfield time
        e = self._earth_ephimeris.at(t) #earth position at time t
        p_au = sf.Distance(m=self.position_eci).au
        v_au_per_d = sf.Velocity(km_per_s=self.velocity_eci/1e3).au_per_d

        barycentric_sat_position = Barycentric(
            e.position.au + p_au,
            e.velocity.au_per_d + v_au_per_d,
            t=t,
        )
        barycentric_sat_position._ephemeris = self._planets_ephem
        # sun_vec = barycentric_sat_position.observe(self._sun_ephimeris).apparent().position.au
        sun_vec = barycentric_sat_position.observe(self._sun_ephimeris).position.au
        
        #to avoid recalculation in same duration
        self._last_sun_vec = sun_vec
        self._last_sun_vec_aq_t_n = self.kernel._t_n
        return sun_vec
    
    def calculate_translational_kinetic_energy(self) -> float:
        """K= 0.5 * M V2  [Joules]"""
        v = self._state[3:6]
        M = self.params.get('M')
        ke:float = (0.5 * M * (v @ v.T))
        return ke

    def update(self):
        if self._propagator == 'sgp4':
            self._spg4_eci_state = self._sat.at(self.kernel.skyfield_time)
            self._state[0:3] = self._spg4_eci_state.position.m
            self._state[3:6] = self._spg4_eci_state.velocity.m_per_s
            self._state_derivative[0:3] = self._state[3:6]
        elif self._propagator == 'euler':
            disturbance_forces = np.zeros(3) #TODO : get disturbance effects
            self.step_system_dynamics(disturbance_forces, {})
        else:
            raise Exception(f'propagator type {self._propagator} is not recognized!')
        
        #save all the state elements we care about for analysis
        saved_state = np.zeros(self.saved_state_size, dtype=np.float64)
        saved_state[:6] = self._state
        oe = self.calculate_orbital_elements()
        saved_state[6:9] = self.position_lonlat
        saved_state[9:13] = [oe.argument_of_periapsis.degrees, oe.longitude_of_ascending_node.degrees, oe.inclination.degrees, oe.mean_anomaly.degrees]
        self.save_chunk_to_memory(saved_state)
        
    def state_update_fcn(self, t:np.ndarray, x:np.ndarray, u:np.ndarray, update_params = None) -> np.ndarray:
        """
        Parameters
        ----------
        x : array
            System state: [x, y, z, vx, vy, vz]
        u : array
            System input: [f1, f2, f3]

        Returns
        -------
        dx : array
            time deterivative of system state
        """
        dx = np.zeros(6)
        
        #TODO this function does nothing rn because sgp4 handles state updates
        # dx[0:3] = x[3:6]
        # x_ecef = self.eci_state_vector_to_ecef(x)
        
        # grav_accel = self.grav_model.get_accel_vector_eci(x_ecef[0:3], x_ecef[3:6])
        
        # dx[3:6] = grav_accel + u
        return dx