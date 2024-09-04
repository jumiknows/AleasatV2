import numpy as np

from pygeomag import GeoMag

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel

from math import degrees

class EarthMagneticFieldModel(AbstractModel):
    """
    Earth magnetic field model based on the WMM2020 spherical harmonics model
    Internally uses the ahrs implementation of WMM2020 https://ahrs.readthedocs.io/en/latest/wmm.html
    * the docs may not be that accurate
    """
    default_name = 'magnetic_field'

    def __init__(self, sim_kernel: AleasimKernel) -> None:
        super().__init__(EarthMagneticFieldModel.default_name, sim_kernel)

        self._wmm = GeoMag()

        self._last_mag_vec_ned = None
        self._last_mag_vec_ned_t_n = -1

        self.logger.info("initialized")

    def connect(self):
        self._odyn: OrbitDynamicsModel = self.kernel.get_model(OrbitDynamicsModel)
    
    @property
    def config_name(self) -> str:
        return 'magnetic_field'

    def get_mag_vector_at_time(self, pos: np.ndarray, year_fraction: float) -> np.ndarray:
        """
        Return the magnetic field vector N,E,D components in units of nT 
            @param: pos (position is lat, long and height [m above ellipsoid surface])
            @param: year-fraction between 2020.0 and 2025.0
        """
        latitude: float = pos[0]
        longitude: float = pos[1]
        height_km: float = pos[2] / 1000
        if year_fraction < 2020.0 or year_fraction > 2025.0:
            raise ValueError(f'year fraction {year_fraction} is out of the valid range for the WMM2020 model 2020.0-2025.0')
        
        mag = np.zeros(3)
        result = self._wmm.calculate(latitude, longitude, height_km, year_fraction, raise_in_warning_zone=True)
        mag[0] = result.x
        mag[1] = result.y
        mag[2] = result.z
        
        return mag
    
    def get_mag_vector(self, pos: np.ndarray) -> np.ndarray:
        """
        Return the magnetic field vector N,E,D components in units of nT 
            @param: pos (position is lat, long and height [km above ellipsoid surface])
        """
        year_fraction = self.kernel.skyfield_time.J #floating point Julian year  from skyfield time
        return self.get_mag_vector_at_time(pos, year_fraction)

    def get_mag_vector_ned(self) -> np.ndarray:
        """Return magnetic field vector in NED frame in [nT] at the current spacecraft location"""
        if self.kernel._t_n == self._last_mag_vec_ned_t_n:
            return self._last_mag_vec_ned
        else:
            #get spacecraft lla coords [lon radians, lat radians, altitude  km]
            lla = self._odyn.position_lla
            #convert lon and lat to degrees from radians
            lla[0] = degrees(lla[0])
            lla[1] = degrees(lla[1])
            
            mag_ned = self.get_mag_vector(lla)
            
            self._last_mag_vec_ned = mag_ned
            self._last_mag_vec_ned_t_n = self.kernel._t_n
            return mag_ned

    def get_mag_vector_body(self) -> np.ndarray:
        """Return magnetic field vector in BODY frame in [nT] at the current spacecraft location"""
        ned = self.get_mag_vector_ned()
        return self.kernel.body_frame.transform_vector_from_frame(ned, self.kernel.ned_frame)
