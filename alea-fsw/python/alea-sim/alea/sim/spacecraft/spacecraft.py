import numpy as np

from alea.sim.kernel.kernel import AleasimKernel, SharedMemoryModelInterface
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.kernel.frames import ReferenceFrame
from .actuators.simple_actuators import SimpleActuator, SimpleMagnetorquer
from .sensors.simple_sensors import SimpleMagSensor, SimpleSunSensor, SimpleGyroSensor
from alea.sim.epa.magnetic_field_model import EarthMagneticFieldModel
from alea.sim.math_lib import Quaternion
from alea.sim.math_lib.math import skew
from alea.sim.algorithms.attitude_determination import wahba_quest
from alea.sim.algorithms.control.pid_control import pd_control_torque
from alea.sim.kernel.scheduler import EventPriority
from alea.sim.kernel.generic.abstract_model import ModelNotFoundError

class Spacecraft(AbstractModel, SharedMemoryModelInterface):
    
    default_name = 'spacecraft'
    def __init__(self, sim_kernel: AleasimKernel, ctrl_sample_period: float = 0.1) -> None:
        super().__init__(Spacecraft.default_name, sim_kernel)

        self.logger.info(f'initalized with spacecraft model')
        self._last_time = self.kernel.time
        self._ctrl_sample_period = ctrl_sample_period
        
        self._use_sensors = False
        self._use_quest = False
    
    @property
    def saved_state_size(self) -> int:
        return 10 + 1 #the last one is attitude estimate error

    @property
    def saved_state_element_names(self) -> list[str]:
        return ['T_rwx', 'T_rwy', 'T_rwz', 'M_mtqx', 'M_mtqy', 'M_mtqz','q_est_0','q_est_1','q_est_2','q_est_3','q_est_error']

    @property
    def config_name(self) -> str:
        return 'spacecraft'

    def connect(self):
        
        self._adyn= self.kernel.get_model("attitude_dynamics")
        self._orbit_dynamics = self.kernel.get_model("orbit_dynamics") 
        self._body_frame : ReferenceFrame = self.kernel.body_frame
        self._eci_frame : ReferenceFrame = self.kernel.eci_frame
        self._magm: EarthMagneticFieldModel = self.kernel.get_model(EarthMagneticFieldModel)
        
        #actuators
        self._rws: list[SimpleActuator] = [] 
        self._rws.append(self.kernel.get_model("rw_x"))
        self._rws.append(self.kernel.get_model("rw_y"))
        self._rws.append(self.kernel.get_model("rw_z"))
        
        self._mtqs: list[SimpleMagnetorquer] = []
        self._mtqs.append(self.kernel.get_model("mtq_x"))
        self._mtqs.append(self.kernel.get_model("mtq_y"))
        self._mtqs.append(self.kernel.get_model("mtq_z"))
        
        #sensors
        try:
            self._mag_sens: SimpleMagSensor = self.kernel.get_model(SimpleMagSensor)
            self._sun_sens: SimpleSunSensor = self.kernel.get_model(SimpleSunSensor)
            self._gyro_sens: SimpleGyroSensor = self.kernel.get_model(SimpleGyroSensor)
            self._use_sensors = True
        except ModelNotFoundError as err:
            self.logger.error(err)
            self._use_sensors = False

        self.kernel.schedule_event(0, EventPriority.HARDWARE_EVENT, self.control_update, self._ctrl_sample_period)
    
    def control_update(self):
        saved_state = np.zeros(self.saved_state_size)
        q_desired = Quaternion.identity()

        magned = self._magm.get_mag_vector_ned()
        magref = self.kernel.eci_frame.transform_vector_from_frame(magned, self.kernel.ned_frame)
        sunref = self._orbit_dynamics.sun_vector_norm
        
        if self._use_sensors:
            mag = self._mag_sens.measure().value
            sun = self._sun_sens.measure().value
            err_rate = self._gyro_sens.measure().value
        else:
            mag = self.kernel.body_frame.transform_vector_from_frame(magned, self.kernel.ned_frame)
            sun = self.kernel.body_frame.transform_vector_from_frame(sunref, self.kernel.eci_frame)
            err_rate = self._adyn.angular_velocity

        if self._use_quest:
            obs = np.array([mag, sun])
            ref = np.array([magref, sunref])
            q_att = wahba_quest(obs, ref).inverse
            saved_state[10] = q_att.abs_angle_diff(self._adyn.quaternion)
        else:
            q_att: Quaternion = self._adyn.quaternion
        saved_state[6:10] = q_att.to_array()

        qerr = q_desired * q_att.inverse

        torque_desired = pd_control_torque(qerr, err_rate, Kp=1e-3, Kd=1e-3)

        supplement_torque: np.ndarray = np.zeros(3)

        for i in range(3):
            self._rws[i].set_torque(torque_desired[i])
            # self.logger.debug(f'set rw {i} torque to {self._rws[i].get_torque()} nM')
            supplement_torque[i] += torque_desired[i] - self._rws[i].get_torque()

        b: np.ndarray = self._magm.get_mag_vector_body()
        m = np.linalg.pinv(skew(b)) @ supplement_torque
        for i in range(3):
            self._mtqs[i].set_moment(m[i])

        saved_state[0:3] = np.array([self._rws[i].get_torque() for i in range(3)])
        saved_state[3:6] = np.array([self._mtqs[i].get_moment() for i in range(3)])
        self.save_chunk_to_memory(saved_state)
