import numpy as np
from enum import IntEnum

from alea.sim.kernel.kernel import AleasimKernel, SharedMemoryModelInterface
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.kernel.generic.powered_unit_model import PoweredUnitModel
from alea.sim.kernel.frames import ReferenceFrame
from alea.sim.spacecraft.actuators.mtq_model import MagnetorquerModel
from alea.sim.spacecraft.actuators.reaction_wheel import ReactionWheelModel
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
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.spacecraft.eps.power_system import PowerSystemModel

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel

class Spacecraft(SharedMemoryModelInterface, AbstractModel):

    class AOCSMode(IntEnum):
        IDLE = 0
        DETUMBLE_MAG = 1
        DETUMBLE_BDOT = 2
        DETUMBLE_BANG_BANG = 3
        SLEW = 4
    
    def __init__(self, sim_kernel: AleasimKernel, ctrl_sample_period: float = 0.1) -> None:
        super().__init__(name="spacecraft", sim_kernel=sim_kernel)

        self._ctrl_sample_period = ctrl_sample_period
        
        self.configure()

    # ==============================================================================
    # Configuration
    # ==============================================================================

    def configure(self):
        self._use_quest = True
        self._use_ekf = True
        self._mode = self.AOCSMode.SLEW
        self._ekf_init = False
        self._q_desired = Quaternion.identity()

        # estimate detumble gain near equator (should be peak)
        # TODO ALEA-1533 analysis for detumbling gain
        self._detumble_gain = 1 #estimate_detumble_gain(incl_mag=0) 
        
        self.quest_weights = np.array([0.5, 0.5]) # TODO optimize weights based on sensor variances / need an algo for this
    
        # create sensors and actuators
        self._mag_sens = SimpleMagSensor('mag_sens', self.kernel, self._ctrl_sample_period)
        self._sun_sens = SimpleSunSensor('sun_sens', self.kernel, self._ctrl_sample_period)
        self._gyro_sens = SimpleGyroSensor('gyro_sens', self.kernel, self._ctrl_sample_period)
        
        self.kernel.add_model(self._mag_sens, parent=self)
        self.kernel.add_model(self._sun_sens, parent=self)
        self.kernel.add_model(self._gyro_sens, parent=self)
        
        self._rws: list[ReactionWheelModel] = []
        self._mtqs: list[MagnetorquerModel] = []
        
        temp = ['x', 'y', 'z']
        for i in range(3):
            normal = np.zeros(3, dtype=np.float64)
            normal[i] = 1.0

            mtq = MagnetorquerModel(name=f"mtq_{temp[i]}", sim_kernel=self.kernel, normal_vec=normal)
            self._mtqs.append(mtq)
            self.kernel.add_model(mtq, parent=self)

            rw = ReactionWheelModel(name=f"rw_{temp[i]}", sim_kernel=self.kernel, spin_axis_body=normal)
            self._rws.append(rw)
            self.kernel.add_model(rw, parent=self)
        
        # create the power system
        self.power_system = PowerSystemModel(self.kernel)
        self.kernel.add_model(self.power_system, parent=self)

    # ==============================================================================
    # Kernel Events
    # ==============================================================================

    def connect(self):
        # TODO cannot use AttitudeDynamicsModel class due to circular import, find a way to fix this
        self._adyn: "AttitudeDyamicsModel" = self.kernel.get_model("attitude_dynamics")
    
        self._orbit_dynamics: OrbitDynamicsModel = self.kernel.get_model(OrbitDynamicsModel) 
        self._body_frame : ReferenceFrame = self.kernel.body_frame
        self._eci_frame : ReferenceFrame = self.kernel.eci_frame
        self._magm: EarthMagneticFieldModel = self.kernel.get_model(EarthMagneticFieldModel)
        
    def start(self):
        #turn on all spacecraft hardware
        self.power_on()
        self.kernel.schedule_event(0, EventPriority.HARDWARE_EVENT, self.control_update, self._ctrl_sample_period)

    # ==============================================================================
    # Simulation Variables
    # ==============================================================================
    
    @property
    def quaternion_estimate(self) -> Quaternion:
        return self._q_est

    @property
    def quaternion_target(self) -> Quaternion:
        return self._q_desired

    # ==============================================================================
    # State Update
    # ==============================================================================

    def power_on(self):
        """Power on all powered units on the spacecraft"""
        for unit in self.get_all_children_of_type(PoweredUnitModel):
            unit.power_on()

    def power_off(self):
        """Power off all powered units on the spacecraft"""
        for unit in self.get_all_children_of_type(PoweredUnitModel):
            unit.power_off()
    
    def control_update(self):
        saved_state = np.zeros(self.saved_state_size)

        magref = self._magm.mag_field_vector_eci
        sunref = self._orbit_dynamics.sun_vector_norm
        
        mag = self._mag_sens.measure().value
        sun = self._sun_sens.measure().value
        gyro_rate = self._gyro_sens.measure().value

        if self._use_quest:
            obs = np.array([mag, sun])
            ref = np.array([magref, sunref])
            self._q_est = wahba_quest(obs, ref, self.quest_weights).inverse
        else:
            self._q_est: Quaternion = self._adyn.quaternion

        if self._mode == self.AOCSMode.SLEW:
            if self._use_ekf:
                #ekf
                q = self._q_est.to_array()
                if not self._ekf_init:
                    self.x = np.array([0, 0, 0, 0, 0, 0, 0, 0, 0])  # Initial state (unit quaternion, zero angular velocity)
                    self.P = np.eye(9) * 0.1  # Initial covariance
                    self.u = np.array([0, 0, 0])  # Control input
                    self.Q = np.eye(9) * 0.01  # Process noise covariance
                    self.R = np.eye(6) * 0.01  # Measurement noise covariance
                    self.J = self._adyn._J #inertia matrix
                    self.ekf = ExtendedKalmanFilter(self.J, self._ctrl_sample_period , self.Q, self.R)
                    self._ekf_init = True
                    self.x[0:6] = np.concatenate([gyro_rate, q[1:]])
                    
                y_k = np.concatenate([gyro_rate, q[1:]])
                x_pred, P_pred = self.ekf.predict(self.x, self.P, self.u) ### I am not sure using "self." is true or I need to use shared memory
                x_upd, P_upd = self.ekf.update(x_pred, P_pred, y_k)
                self.x = x_upd
                self.P = P_upd
                
                #recreate the full quaternion from the estimated reduced quaternion
                q0 = 1- self.x[3]**2 - self.x[4]**2 - self.x[5]**2
                if q0 > 0:
                    q = np.array([np.sqrt(q0), self.x[3], self.x[4], self.x[5]])
                else:
                    q = np.array([0.0, self.x[3], self.x[4], self.x[5]])
                norm = np.linalg.norm(q)
                q/=np.linalg.norm(q)

                if not np.isnan(norm):
                    self._q_est = Quaternion(q)
                    gyro_rate = self.x[0:3]
                else:
                    self.logger.warning(f"EKF estimate contains NaN. x:{self.x} P: {self.P}")

            qerr = self._q_desired * self._q_est.inverse

            torque_desired = pd_control_torque(qerr, gyro_rate, Kp=1e-3, Kd=1e-3)

            supplement_torque: np.ndarray = np.zeros(3)

            for i in range(3):
                self._rws[i].set_torque_command(torque_desired[i])
                # self.logger.debug(f'set rw {i} torque to {self._rws[i].get_torque()} nM')
                supplement_torque[i] += torque_desired[i] - self._rws[i].torque

            b: np.ndarray = self._magm.mag_field_vector_body * 1E-9
            m = np.linalg.pinv(skew(b)) @ supplement_torque
            for i in range(3):
                # TODO ALEA-2400 support different mapping between moment and duty cycle
                self._mtqs[i].set_duty_cycle(m[i]/self._mtqs[i].cfg.saturation_moment * 100.0)

        elif self._mode == self.AOCSMode.DETUMBLE_MAG:
            mag = mag*1E-9
            m = magnetic_detumble_control(gyro_rate, mag, k_gain=self._detumble_gain)
            for i in range(3):
                # TODO ALEA-2400 support different mapping between moment and duty cycle
                self._mtqs[i].set_duty_cycle(m[i]/self._mtqs[i].cfg.saturation_moment * 100.0)
        # TODO implement detumble bdot and bang bang
        # calculate and filter mag derivative
        
        saved_state[0:4] = self._q_desired.to_array()
        saved_state[4:8] = self._q_est.to_array()
        saved_state[8:11] = self._adyn.quaternion.to_euler() - self._q_est.to_euler()
        saved_state[11:14] = self._q_desired.to_euler() - self._adyn.quaternion.to_euler()
        saved_state[14] = self._q_est.abs_angle_diff(self._adyn.quaternion)
        saved_state[15] = self._adyn.quaternion.abs_angle_diff(self._q_desired)

        self.save_chunk_to_memory(saved_state)

    @property
    def saved_state_size(self) -> int:
        return len(self.saved_state_element_names)

    @property
    def saved_state_element_names(self) -> list[str]:
        if not hasattr(self, '_saved_state_element_names'):
            self._saved_state_element_names = ['q_target_0', 'q_target_1', 'q_target_2', 'q_target_3',
                'q_est_0', 'q_est_1', 'q_est_2', 'q_est_3',
                'ake_x', 'ake_y', 'ake_z',
                'ape_x', 'ape_y', 'ape_z',
                'ake_abs_err', 'ape_abs_err']
        return self._saved_state_element_names
