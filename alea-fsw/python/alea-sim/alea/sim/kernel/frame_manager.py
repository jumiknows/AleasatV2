from enum import Enum
from weakref import proxy
import logging

import numpy as np

from skyfield.elementslib import OsculatingElements

from alea.sim.epa.frame_conversions import ecef_to_ned_rot_mat, eci_to_ecef_rot_mat, eci_to_orbit_rot_mat
from alea.sim.kernel.frames import FrameTransformation, ReferenceFrame, Universe
from alea.sim.math_lib import Quaternion
from alea.sim.kernel.scheduler import EventPriority
from alea.sim.kernel.generic.abstract_model import ModelNotFoundError

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from alea.sim.kernel.kernel import AleasimKernel

class FrameManager():
    """
    handles updating the transforms in the universe mainly eci, body, ecef and ned
    """

    class FrameEnum(Enum):
        REFERENCE = "ECI"

        ECI  = "ECI"
        BODY = "BODY"
        ECEF = "ECEF"
        NED = "NED"
        ORBIT = "ORBIT"

    default_name = 'frame_manager'

    def __init__(self, sim_kernel: "AleasimKernel") -> None:
        self._sim_kernel = proxy(sim_kernel)
        self._universe: Universe = Universe(Quaternion)
        self._create_frames()
        self.logger = logging.getLogger("frame_manager")
        
        #whether or not to update the BODY and NED frames
        #False by defualt, until position/orientation knowledge of the spacecraft is avaialable
        #ecef will still be updated as the relation between eci and ecef does not depend on the satellite
        self._update_spacecraft_enabled = False
        
        self._last_update_time: float = -1
        
        self._sim_kernel.schedule_event(self._sim_kernel.time, 0, self._grab_models)
        self._sim_kernel.schedule_event(self._sim_kernel.time, EventPriority.COORDINATE_EVENT, self.update_frames, period=self._sim_kernel.timestep)

    def _create_frames(self):
        self._eci_frame = self._universe.create_frame(self.FrameEnum.ECI.value)
        self._ecef_frame = self._universe.create_frame(self.FrameEnum.ECEF.value, derived_from=self.FrameEnum.ECI.value)
        self._body_frame = self._universe.create_frame(self.FrameEnum.BODY.value, derived_from=self.FrameEnum.ECI.value)
        self._ned_frame = self._universe.create_frame(self.FrameEnum.NED.value, derived_from=self.FrameEnum.ECEF.value)
        self._orbit_frame = self._universe.create_frame(self.FrameEnum.ORBIT.value, derived_from=self.FrameEnum.ECI.value)

    def _grab_models(self):
        """
        this function should be called after attitude dynamics and orbit dynamics are added to the sim
        """
        
        #here we catch the error raised if attitude and orbit dynamics do not exist
        #we do this because we can still update some frames with orientation or position knowledge of the satellite
        #namely the relation between eci and ecef does not depend on the satellite
        try:
            self._adyn = self._sim_kernel.get_model('attitude_dynamics')
            self._odyn = self._sim_kernel.get_model('orbit_dynamics')
            self._update_spacecraft_enabled = True
        except ModelNotFoundError as errmsg:
            self._update_spacecraft_enabled = False

    @property
    def universe(self) -> Universe:
        """Storage object for frame tree"""
        return self._universe
    
    @property
    def eci_frame(self) -> ReferenceFrame:
        """Earth centered inertial frame"""
        return self._eci_frame

    @property
    def body_frame(self) -> ReferenceFrame:
        """Spacecraft body frame"""
        return self._body_frame

    @property
    def ned_frame(self) -> ReferenceFrame:
        """North East Down frame"""
        return self._ned_frame

    @property
    def ecef_frame(self) -> ReferenceFrame:
        """Earth centered Earth fixed frame"""
        return self._ecef_frame

    @property
    def orbit_frame(self) -> ReferenceFrame:
        """Orbital frame"""
        return self._orbit_frame
    
    @property
    def last_update_time(self) -> float:
        """
        last update time in utc seconds since sim start
        will return -1 if update_frames() has not been called yet
        """
        return self._last_update_time

    def update_frames(self):
        """
        update the ECEF frame
        if self._update_spacecraft_enabled, also update the BODY and NED frames
        (see comments in __init__ and _grab_models)
        """
        #eci->ecef transformation update
        q_eciecef = Quaternion.from_dcm(eci_to_ecef_rot_mat(self._sim_kernel.gmst_rad))
        self._ecef_frame.derive_from(self._eci_frame, FrameTransformation(q_eciecef, np.zeros(3)))
        # self.logger.log(logging.DEBUG, f'updated ECI->ECEF relation to \n{self._eci_frame.get_transformation_to(self._ecef_frame)}')
        
        if self._update_spacecraft_enabled:
            q_ecibody = self._adyn.quaternion
            r_eci = self._odyn.position_eci
            r_ecef = self._odyn.position_ecef

            #eci->body update
            self._body_frame.derive_from(self._eci_frame, FrameTransformation(q_ecibody, r_eci))
    
            #ecef->ned update
            lla = self._odyn.position_lonlat
            lon = lla[0]
            lat = lla[1]
            q_ecefned = Quaternion.from_dcm(ecef_to_ned_rot_mat(lon, lat))
            self._ned_frame.derive_from(self._ecef_frame, FrameTransformation(q_ecefned, r_ecef))
            
            #update orbital frame
            #https://rhodesmill.org/skyfield/elements.html
            elements: OsculatingElements = self._odyn.calculate_orbital_elements()
            raan = elements.longitude_of_ascending_node.radians #also known as right ascension of ascending node ??
            incl = elements.inclination.radians
            mean_anomaly = elements.mean_anomaly.radians
            argp = elements.argument_of_periapsis.radians #argument of periapsis is also known as argument of perigee for geocentric orbits???
            
            #ok now we make the transformation and update the refeerence frame object
            #rotation matrix from eci to orbit frame
            #position is just the spacecraft position in eci
            q_eciorbit = Quaternion.from_dcm(eci_to_orbit_rot_mat(raan, incl, argp, mean_anomaly))
            self._orbit_frame.derive_from(self._eci_frame, FrameTransformation(q_eciorbit, r_eci))
        
        self._last_update_time: float = float(self._sim_kernel.time)
        
    