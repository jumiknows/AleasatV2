import numpy as np
from enum import IntEnum

from alea.sim.kernel.kernel import AleasimKernel, SharedMemoryModelInterface
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.kernel.frames import ReferenceFrame
from alea.sim.spacecraft.actuators.simple_actuators import SimpleActuator, SimpleMagnetorquer
from alea.sim.spacecraft.sensors import SimpleGyroSensor, SimpleMagSensor, SimpleSunSensor
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.math_lib import Quaternion
from alea.sim.math_lib.math import skew
from alea.sim.algorithms.attitude_determination import wahba_quest
from alea.sim.algorithms.control.pid_control import pd_control_torque
from alea.sim.algorithms.control.detumble import estimate_detumble_gain, bdot_detumble_control, magnetic_detumble_control, bang_bang_bdot_detumble_control
from alea.sim.kernel.scheduler import EventPriority
from alea.sim.kernel.generic.abstract_model import ModelNotFoundError
from alea.sim.algorithms.attitude_determination import ExtendedKalmanFilter

class Spacecraft(AbstractModel, SharedMemoryModelInterface):

    class AOCSMode(IntEnum):
        IDLE = 0
        DETUMBLE_MAG = 1
        DETUMBLE_BDOT = 2
        DETUMBLE_BANG_BANG = 3
        SLEW = 4
    
    def __init__(self, sim_kernel: AleasimKernel, ctrl_sample_period: float = 0.1) -> None:
        super().__init__("spacecraft", sim_kernel)

        self.logger.info(f'initalized with spacecraft model')
        self._last_time = self.kernel.time
        self._ctrl_sample_period = ctrl_sample_period
        
        self._use_sensors = True
        self._use_quest = True
        self._use_ekf = True
        self._mode = self.AOCSMode.SLEW

        self._q_desired = Quaternion.identity()

        self._detumble_gain = estimate_detumble_gain(incl_mag=0) #estimate detumble gain near equator (should be peak)

        self._ekf_init = False
    
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

            self._mag_sens.power_on()
            self._sun_sens.power_on()
            self._gyro_sens.power_on()
            self.quest_weights = np.array([0.5, 0.5]) # TODO optimize weights based on sensor variances / need an algo for this
        except ModelNotFoundError as err:
            self.logger.error(err)
            self._use_sensors = False

        self.kernel.schedule_event(0, EventPriority.HARDWARE_EVENT, self.control_update, self._ctrl_sample_period)
    
    def control_update(self):
        saved_state = np.zeros(self.saved_state_size)
        q_desired = Quaternion.identity()

        magref = self._magm.mag_field_vector_eci
        sunref = self._orbit_dynamics.sun_vector_norm
        
        if self._use_sensors:
            mag = self._mag_sens.measure().value
            sun = self._sun_sens.measure().value
            err_rate = self._gyro_sens.measure().value
        else:
            mag = self._magm.mag_field_vector_body
            sun = self.kernel.body_frame.transform_vector_from_frame(sunref, self.kernel.eci_frame)
            err_rate = self._adyn.angular_velocity

        if self._use_quest:
            obs = np.array([mag, sun])
            ref = np.array([magref, sunref])
            q_att = wahba_quest(obs, ref, self.quest_weights).inverse
            saved_state[10] = q_att.abs_angle_diff(self._adyn.quaternion)
        else:
            q_att: Quaternion = self._adyn.quaternion

        if self._mode == self.AOCSMode.SLEW:
            if self._use_ekf:
                #ekf
                q = q_att.to_array()
                if not self._ekf_init:
                    self.x = np.array([0, 0, 0, 0, 0, 0, 0, 0, 0])  # Initial state (unit quaternion, zero angular velocity)
                    self.P = np.eye(9) * 0.1  # Initial covariance
                    self.u = np.array([0, 0, 0])  # Control input
                    self.Q = np.eye(9) * 0.01  # Process noise covariance
                    self.R = np.eye(6) * 0.01  # Measurement noise covariance
                    self.J = self._adyn._J #inertia matrix
                    self.ekf = ExtendedKalmanFilter(self.J, self._ctrl_sample_period , self.Q, self.R)
                    self._ekf_init = True
                    self.x[0:6] = np.concatenate([err_rate, q[1:]])
                    
                y_k = np.concatenate([err_rate, q[1:]])
                x_pred, P_pred = self.ekf.predict(self.x, self.P, self.u) ### I am not sure using "self." is true or I need to use shared memory
                x_upd, P_upd = self.ekf.update(x_pred, P_pred, y_k)
                self.x = x_upd
                self.P = P_upd
                
                #recreate the full quaternion from the estimated reduced quaternion
                q = np.array([np.sqrt(1- self.x[3]**2 - self.x[4]**2 - self.x[5]**2), self.x[3], self.x[4], self.x[5]])
                norm = np.linalg.norm(q)
                q/=np.linalg.norm(q)

                if not np.isnan(norm):
                    q_att = Quaternion(q)
                    err_rate = self.x[0:3]
                else:
                    self.logger.warn(f"EKF estimate contains NaN. x:{self.x} P: {self.P}")

            saved_state[6:10] = q_att.to_array()

            qerr = self._q_desired * q_att.inverse

            torque_desired = pd_control_torque(qerr, err_rate, Kp=1e-3, Kd=1e-3)

            supplement_torque: np.ndarray = np.zeros(3)

            for i in range(3):
                self._rws[i].set_torque(torque_desired[i])
                # self.logger.debug(f'set rw {i} torque to {self._rws[i].get_torque()} nM')
                supplement_torque[i] += torque_desired[i] - self._rws[i].get_torque()

            b: np.ndarray = self._magm.mag_field_vector_body
            m = np.linalg.pinv(skew(b)) @ supplement_torque
            for i in range(3):
                self._mtqs[i].set_moment(m[i])

        elif self._mode == self.AOCSMode.DETUMBLE_MAG:
            mag = mag*1E-9
            m = magnetic_detumble_control(err_rate, mag, k_gain=self._detumble_gain)
            for i in range(3):
                self._mtqs[i].set_moment(m[i])
        # TODO implement detumble bdot and bang bang
        # calculate and filter mag derivative

        saved_state[0:3] = np.array([self._rws[i].get_torque() for i in range(3)])
        saved_state[3:6] = np.array([self._mtqs[i].get_moment() for i in range(3)])
        self.save_chunk_to_memory(saved_state)
