from pathlib import Path
from enum import IntEnum
import dataclasses

import numpy as np

import skyfield.timelib
import skyfield

from alea.sim.kernel.kernel import AleasimKernel, SharedMemoryModelInterface
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.kernel.generic.powered_unit_model import PoweredUnitModel
from alea.sim.kernel.frames import ReferenceFrame
from alea.sim.spacecraft.actuators.mtq_model import MagnetorquerModel
from alea.sim.spacecraft.actuators.reaction_wheel import ReactionWheelModel
from alea.sim.spacecraft.sensors import SimpleMagSensor, SimpleSunSensor, GyroSensor, SimpleGyroSensor
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.math_lib import Quaternion
from alea.sim.math_lib.math import skew
from alea.sim.algorithms.attitude_determination import wahba_quest
from alea.sim.algorithms.control.pid_control import pd_control_torque
from alea.sim.algorithms.control.detumble import estimate_detumble_gain, bdot_detumble_control, magnetic_detumble_control, bang_bang_bdot_detumble_control
from alea.sim.algorithms.control.momentum_dumping import momentum_dumping_control
from alea.sim.kernel.scheduler import EventPriority
from alea.sim.kernel.generic.abstract_model import ModelNotFoundError
from alea.sim.algorithms.attitude_determination import ExtendedKalmanFilter
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.algorithms.guidance.location_pointing import compute_location_pointing_quaternion, compute_target_quaternion
from alea.sim.algorithms.control.torque_allocation import simple_torque_allocation
from alea.sim.math_lib.shapes import WGS84Earth
from alea.sim.kernel.time_utils import skyfield_time_to_gmst_radians
from alea.sim.epa.frame_conversions import eci_to_ecef_rot_mat
from alea.sim.configuration import Configurable
from alea.sim.algorithms.filters.low_pass_filter import LowPassFilter

# type checking hack
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel

# wgs84 earth surface model for use in aocs target pointing calculations
wgs84 = WGS84Earth()

@dataclasses.dataclass
class AOCSConfig:
    k_p: np.ndarray | float
    k_d: np.ndarray | float
    k_detumble: np.ndarray | float
    rw_saturation_threshold: float
    quest_weights: np.ndarray
    k_momentum_dumping: float

    # set to true to use the 'ground truth' of the simulation as the estimated state
    # this is cheating, only use this for testing
    # overrides usage of QUEST, EKF and low pass filters
    enable_ideal_estimation: bool
    
    enable_ekf: bool = True
    enable_sensor_lpf: bool = False

    def __post_init__(self):
        #convert list[float] to np.ndarry
        if isinstance(self.k_p, list):
            self.k_p = np.diag(self.k_p)
        if isinstance(self.k_d, list):
            self.k_d = np.diag(self.k_d)
        self.k_detumble = np.array(self.k_detumble)
        self.quest_weights = np.array(self.quest_weights)

class AOCSModel(Configurable[AOCSConfig], SharedMemoryModelInterface, AbstractModel):
    """
    The Attitude and Orbit Control System model implements all AOCS functionality
        - Instantiates AOCS sensor and actuator hardware models
        - Implements the AOCS closed control loop
            - Guidance/Naviagtion
            - Sensor reading and Estimation
            - Control action computation
            - Actuator commands
            
    Limitations:
        - Not all software and hardware delays are accounted for in the control loop
    """

    class AOCSMode(IntEnum):
        IDLE = 0
        DETUMBLE_MAG = 1
        # DETUMBLE_BDOT = 2 # TODO other detumbling algos
        # DETUMBLE_BANG_BANG = 3
        MOMENTUM_DUMP = 2
        POINTING_SUN_A = 3
        POINTING_SUN_B = 4
        POINTING_MIN_POWER_GEN = 5
        # POINTING_ANTENNA = 6 # TODO support antenna pointing mode
        POINTING_CAMERA_NADIR = 7
        POINTING_CAMERA_TARGET = 8
        
        POINTING_QUAT = 20

    def __init__(self, sim_kernel: AleasimKernel, ctrl_sample_period: float = 0.1, 
                 cfg: str | Path | dict | AOCSConfig = "aocs") -> None:
        super().__init__(name="aocs", sim_kernel=sim_kernel, cfg=cfg, cfg_cls=AOCSConfig)

        self._ctrl_sample_period = ctrl_sample_period
        
        self.configure()

    # ==============================================================================
    # Configuration
    # ==============================================================================

    def configure(self):
        self._ekf_init: bool = False

        self._mode: AOCSModel.AOCSMode = AOCSModel.AOCSMode.IDLE
        self._q_target: Quaternion = Quaternion.identity()
        self._state_estimate: np.ndarray = np.zeros(7)
        self._ape_estimate: np.ndarray = np.zeros(3)
        self._t_mtq = np.zeros(3)
        
        # ekf state
        self._ekf_x = np.zeros(9)

        # torque allocation weights, by defaults all ones
        self.allocation_weights = None
        self.allocation_mask = None
        
        self._ctrl_sample_frequency: int = int(1/self._ctrl_sample_period)
    
        #create sensors and actuatorsP
        self._mag_sens = SimpleMagSensor('mag_sens', self.kernel, self._ctrl_sample_frequency)
        self._sun_sens = SimpleSunSensor('sun_sens', self.kernel, self._ctrl_sample_frequency)
        
        # TODO add back after ALEA-2444 and ALEA-2403, replace simple model
        self._gyro_sens = SimpleGyroSensor('gyro_sens', self.kernel, self._ctrl_sample_frequency)
        # self._gyro_sens = GyroSensor('gyro_sens', self.kernel, self._ctrl_sample_frequency)

        # Sensor filters
        self._mag_sens_filter = LowPassFilter(filter_name='avg', sample_rate=None, window_size=5, order=None, cutoff_freq=None)
        self._sun_sens_filter = LowPassFilter(filter_name='avg', sample_rate=None, window_size=5, order=None, cutoff_freq=None)
        self._gyro_sens_filter = LowPassFilter(filter_name='avg', sample_rate=None, window_size=5, order=None, cutoff_freq=None)
        
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

    # ==============================================================================
    # Kernel Events
    # ==============================================================================

    def connect(self):
        self._adyn: "AttitudeDynamicsModel" = self.kernel.get_model("attitude_dynamics")  
        self._orbit_dynamics: OrbitDynamicsModel = self.kernel.get_model(OrbitDynamicsModel) 
        self._body_frame : ReferenceFrame = self.kernel.body_frame
        self._eci_frame : ReferenceFrame = self.kernel.eci_frame
        self._magm: EarthMagneticFieldModel = self.kernel.get_model(EarthMagneticFieldModel)
        
    def start(self):
        #turn on all AOCSModel hardware
        self.power_on()
        self.kernel.schedule_event(0, EventPriority.CONTROL_EVENT, self.aocs_executive_task, self._ctrl_sample_period)

    # ==============================================================================
    # Simulation Variables
    # ==============================================================================

    @property
    def q_true(self) -> Quaternion:
        """Ground truth quaternion of the AttitudeDynamicsModel"""
        return self._adyn.quaternion

    @property
    def q_est(self) -> Quaternion:
        """Estimated quaternion of attitude control loop"""
        return self._q_est

    @property
    def q_target(self) -> Quaternion:
        """Target/desired quaternion of attitude control loop"""
        return self._q_target

    @property
    def abs_pointing_error(self) -> float:
        # In degrees
        return self._adyn.quaternion.abs_angle_diff(self._q_target)

    # ==============================================================================
    # Guidance
    # ==============================================================================

    def compute_quaternion(self, mode: AOCSMode) -> Quaternion:
        """Compute target for sun or nadir modes @ current AOCSModel position"""
        if mode == AOCSModel.AOCSMode.POINTING_SUN_A:
            #construct the AOCSModel fixed vectors and sun vector
            vector_target = self._orbit_dynamics.sun_vector_norm
            vector_spacecraft = np.array([-1.0, 1.0, -1.0]) #the -X, +Y, -Z solar panels intersect at this corner
            vector_spacecraft /= np.linalg.norm(vector_spacecraft)
        elif mode == AOCSModel.AOCSMode.POINTING_SUN_B:
            vector_target = self._orbit_dynamics.sun_vector_norm
            vector_spacecraft = np.array([-1.0, -1.0, -1.0])  #the -X, -Y, -Z solar panels intersect at this corner
            vector_spacecraft /= np.linalg.norm(vector_spacecraft)
        elif mode == AOCSModel.AOCSMode.POINTING_MIN_POWER_GEN:
            vector_target = self._orbit_dynamics.sun_vector_norm
            vector_spacecraft = np.array([0, 0, 1.0])  #pointing a non solar panel face at the sun is sun avoding
            vector_spacecraft /= np.linalg.norm(vector_spacecraft)
        elif mode == AOCSModel.AOCSMode.POINTING_CAMERA_NADIR:
            vector_target = -1.0 * self._orbit_dynamics.position_eci
            vector_spacecraft = np.array([1.0, 0.0, 0.0]) #we will point +X (camera) face nadir
        else:
            self.logger.warning(f"Cannot set target profile for {mode}")
            return None
        qt: Quaternion = compute_target_quaternion(vector_target, vector_spacecraft)

        return qt

    def compute_camera_target_quaternion(self, latlon: tuple[float, float], t: skyfield.timelib.Time, sc_pos_eci: np.ndarray = None) -> Quaternion:
        """Compute target for camera to point at earth location from a given AOCSModel position sc_pos_eci
            if sc_pos_eci is None, will use current orbit_dynamics.position_eci"""
        lat, lon = latlon
        sc_axis = np.array([1.0, 0.0, 0.0])
        phi, theta = wgs84.geographic_to_spherical(lat, lon)
        x, y, z = wgs84.cartesian_surface_point(phi, theta)
        r_ecef = np.array([x,y,z])
        if t is None:
            t = self.kernel.skyfield_time
        gmst_rad = skyfield_time_to_gmst_radians(t)
        rotmat = eci_to_ecef_rot_mat(gmst_rad).T # Tranpose == inverse (since we're going from ECEF to ECI)
        r_eci =  rotmat @ r_ecef

        if sc_pos_eci is None:
            sc_pos_eci = self._orbit_dynamics.position_eci

        qt = compute_location_pointing_quaternion(sc_pos_eci, r_eci, sc_axis)

        return qt

    # ==============================================================================
    # State Estimation
    # ==============================================================================

    def tick_estimate(self):
        """
        State estimation function of the control loop
            1. computes reference vectors
            2. reads sensors
            3. Calculates quaternio with QUEST
            4. Runs EKF update and predict steps to gyro measurement and QUEST quaternion
        """
        
        # compute ECI reference vectors from mag and sun models
        self._magref = self._magm.mag_field_vector_eci * 1E-9
        self._sunref = self._orbit_dynamics.sun_vector_norm
        
        # sensor measurement
        self._mag = self._mag_sens.measure().value * 1E-9
        self._sun = self._sun_sens.measure().value
        self._gyro_rate = self._gyro_sens.measure().value

        # Only filter the true (noisy) signal
        if self.cfg.enable_sensor_lpf:
            self._mag = self._mag_sens_filter.filter_output(self._mag_sens.state_array[:, 3:6])
            self._sun = self._sun_sens_filter.filter_output(self._sun_sens.state_array[:, 3:6])
            self._gyro_rate = self._gyro_sens_filter.filter_output(self._gyro_sens.state_array[:, 3:6])

        # compute quest quaternion
        obs = np.array([self._mag, self._sun])
        ref = np.array([self._magref, self._sunref])
        self._q_est = wahba_quest(obs, ref, self.cfg.quest_weights).inverse

        # run the ekf if enabled
        if self.cfg.enable_ekf:
            q = self._q_est.to_array()

            if not self._ekf_init:
                # Initial state (unit quaternion, zero angular velocity)
                self._ekf_x = np.array([0, 0, 0, 0, 0, 0, 0, 0, 0], dtype=np.float64)
                self._ekf_P = np.eye(9) * 0.1  # Initial covariance
                self._ekf_u = np.array([0.0, 0.0, 0.0])  # Control input
                Q = np.eye(9) * 0.1  # Process noise covariance
                R = np.eye(6) * 0.1  # Measurement noise covariance
                self.ekf = ExtendedKalmanFilter(self._adyn.cfg.J, self._ctrl_sample_period , Q, R)
                self._ekf_x[0:6] = np.concatenate([self._gyro_rate, q[1:]])
        
                self._ekf_init = True
            
            # compose measurement vector from gyro measurement and quest output
            y_k = np.concatenate([self._gyro_rate, q[1:]])
            #ekf predict step, predict our state estimate and covariance based on dynamics
            x_pred, P_pred = self.ekf.predict(self._ekf_x, self._ekf_P, self._ekf_u)
            # ekf update step, update our state estimate based on sensor measurement and quest
            x_upd, P_upd = self.ekf.update(x_pred, P_pred, y_k)
            self._ekf_x = x_upd
            self._ekf_P = P_upd
            
            #recreate the full quaternion from the estimated reduced quaternion
            q0 = 1- self._ekf_x[3]**2 - self._ekf_x[4]**2 - self._ekf_x[5]**2
            
            # only compute the square root if a valid quaternion is estimated
            if q0 > 0:
                q = np.array([np.sqrt(q0), self._ekf_x[3], self._ekf_x[4], self._ekf_x[5]])
            else:
                #otherwise set to quest output
                self._ekf_x[3:6] = q[1:]

            #normalize the quaternion
            norm = np.linalg.norm(q)
            q/=norm

            # sanity check
            if not np.isnan(q0) or q0 > 0:
                self._q_est = Quaternion(q)
                self._gyro_rate = self._ekf_x[0:3] + self._ekf_x[6:9]
            else:
                self.logger.warning(f"EKF estimate contains NaN. x:{self._ekf_x} P: {self._ekf_P}")
                self._ekf_init = False

    # ==============================================================================
    # State Control
    # ==============================================================================

    def tick_control(self):
        """
        State function of the control loop
            1. Based on mode, computes desired torques and moments for attitude control or detumbling
            2. Allocates torque among actuators
            3. Sends torque and moment commands to actuators
        """
        if self._mode == self.AOCSMode.DETUMBLE_MAG:
            m = magnetic_detumble_control(self._gyro_rate, self._mag, k_gain=self.cfg.k_detumble)
            for i in range(3):
                # TODO ALEA-2400 support different mapping between moment and duty cycle
                self._mtqs[i].set_duty_cycle(m[i]/self._mtqs[i].cfg.saturation_moment * 100.0)

        # TODO implement detumble bdot and bang bang
        # calculate and filter mag derivative
        elif self._mode == self.AOCSMode.MOMENTUM_DUMP:
            # desaturate the wheels while keeping spacecraft angular rates within safe limits
            # TODO stability analysis
            h_wheels = np.array([rw.angular_momentum for rw in self._rws])

            m = magnetic_detumble_control(self._gyro_rate, self._mag, k_gain=self.cfg.k_detumble)
            rw_trq_cmd, mtq_m_cmd = momentum_dumping_control(h_wheels, self._mag, k_gain=self.cfg.k_momentum_dumping)
            mtq_m_cmd += m

            # set actuator commands
            for i in range(3):
                self._rws[i].set_torque_command(rw_trq_cmd[i])
                self._mtqs[i].set_duty_cycle(mtq_m_cmd[i]/self._mtqs[i].cfg.saturation_moment * 100.0)
        else:
            self._state_estimate = np.concatenate([self._q_est.to_array(), self._gyro_rate])
            
            # compute error
            if not self.cfg.enable_ideal_estimation:
                q_err = self._q_target.inverse * self._q_est
                rate_err = self._gyro_rate
            else:
                # only use this for testing
                # self._adyn.quaternion is the ground truth
                q_err = self._q_target.inverse * self._adyn.quaternion
                rate_err = self._adyn.angular_velocity
            
            if self._mode != self.AOCSMode.IDLE:
                # compute desired control torque based on errror
                pid_torque = pd_control_torque(q_err, rate_err, Kp=self.cfg.k_p, Kd=self.cfg.k_d)
                b = self._mag

                # determine if reaction wheels are saturated
                h_wheels = np.zeros(3)
                rw_saturation_flags: list[bool] = []
                for i in range(3):
                    rw = self._rws[i]
                    h_wheels += rw.angular_momentum_vector
                    if (np.abs(rw.angular_velocity) > np.abs(self.cfg.rw_saturation_threshold * rw.cfg.max_speed_rads)) \
                        and np.sign(pid_torque[i]) != np.sign(rw.angular_velocity) \
                            and rw.cfg.enable_momentum_saturation:
                        rw_saturation_flags.append(True)
                    else:
                        rw_saturation_flags.append(False)
                
                # allocate torque depending on reaction wheel state
                rw_trq_cmd = np.zeros(3)
                mtq_m_cmd = np.zeros(3)
                if True in rw_saturation_flags:
                    allocation_weights = np.ones(6)
                    allocation_weights[3:6] = np.array([not b for b in rw_saturation_flags]).astype('float')
                    rw_trq_cmd, mtq_m_cmd = simple_torque_allocation(pid_torque, b, weights = self.allocation_weights)
                else:
                    rw_trq_cmd, mtq_m_cmd = simple_torque_allocation(pid_torque, b, weights = self.allocation_weights)

                # set actuator commands
                for i in range(3):
                    self._rws[i].set_torque_command(rw_trq_cmd[i])
                    self._mtqs[i].set_duty_cycle(mtq_m_cmd[i]/self._mtqs[i].cfg.saturation_moment * 100.0)

                self._t_mtq = np.zeros(3)
                for i in range(3):
                    self._t_mtq += - skew(b) @ self._mtqs[i].get_moment()

                # TODO ALEA-2731 investigate EKF instability for non-zero u (applied torque)
                # self.u = self._t_mtq + np.array([rw.torque for rw in self._rws])

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

    def set_mode(self, mode: AOCSMode, t_delay:float = 0, 
                 latlon: tuple[float, float] = None,
                 t_target: skyfield.timelib.Time = None, 
                 sc_pos_eci: np.ndarray = None,
                 q: Quaternion = None) -> None:
        """Set a aocs mode an optional delay for it to take effect."""
        
        if mode > AOCSModel.AOCSMode.MOMENTUM_DUMP and mode < AOCSModel.AOCSMode.POINTING_CAMERA_TARGET:
            qt = self.compute_quaternion(mode)
        elif mode == AOCSModel.AOCSMode.POINTING_CAMERA_TARGET:
            qt = self.compute_camera_target_quaternion(latlon, t_target, sc_pos_eci)
        elif mode == AOCSModel.AOCSMode.POINTING_QUAT:
            if q is None:
                raise ValueError(f"q cannot be none for {mode} mode!")
            qt = q
        else:
            qt = None
        
        if t_delay > 0.0:
            self.kernel.schedule_event(t_delay, EventPriority.CONTROL_EVENT, self._set_mode , argument=(mode, qt))
        else:
            self._set_mode(mode, qt)

    def _set_mode(self, mode: AOCSMode, target: Quaternion = None) -> None:
        """for internal usage only"""
        if self._mode != mode:
            # reset state estimator, this improved performance
            # TODO investigate why
            self._ekf_init = False
        if target is not None:
            self._q_target = target
        self._mode = mode
        
    def _update_current_target(self):
        """Update existing sun or nadir pointing target"""
        m = self._mode
        if m == AOCSModel.AOCSMode.POINTING_SUN_A \
            or m == AOCSModel.AOCSMode.POINTING_SUN_B \
            or m == AOCSModel.AOCSMode.POINTING_CAMERA_NADIR \
            or m == AOCSModel.AOCSMode.POINTING_MIN_POWER_GEN:

            self._q_target = self.compute_quaternion(m)
            
    def aocs_executive_task(self):
        """
        Main control loop task
            1. Updates control target based on mode
            2. estimates state
            3. computes control action and sends actuator commands
            
        For simulation purposes:
            4. saves state to shared memory
        """
        self._update_current_target()
        self.tick_estimate()
        self.tick_control()
        
        self.save_state()

    # ==============================================================================
    # Saved State
    # ==============================================================================

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
                'ake_abs_err', 'ape_abs_err',
                'rate_est_x', 'rate_est_y', 'rate_est_z','mode',
                't_mtq_x','t_mtq_y','t_mtq_z',
                'ekf_wx', 'ekf_wy', 'ekf_wz',
                'ekf_q0','ekf_q1', 'ekf_q2', 'ekf_q3',
                'ekf_biasx', 'ekf_biasy', 'ekf_biasz'
                ]
        return self._saved_state_element_names

    def save_state(self):
        saved_state = np.zeros(self.saved_state_size)
        saved_state[0:4] = self._q_target.to_array()
        saved_state[4:8] = self._q_est.to_array()
        saved_state[8:11] = (self._q_est * self._adyn.quaternion.inverse).to_euler()

        # during detumbling mode, pointing error "does not matter"
        if self._mode > AOCSModel.AOCSMode.MOMENTUM_DUMP:
            self._ape_estimate = (self._adyn.quaternion * self._q_target.inverse).to_euler()
            saved_state[11:14] = self._ape_estimate
            saved_state[15] = self._adyn.quaternion.abs_angle_diff(self._q_target)

        saved_state[14] = self._q_est.abs_angle_diff(self._adyn.quaternion)
        saved_state[16:19] = self._gyro_rate
        saved_state[19] = int(self._mode)
        saved_state[20:23] = self._t_mtq
        saved_state[23:26] = self._ekf_x[0:3]
        saved_state[26] = self._q_est.to_array()[0]
        saved_state[27:33] = self._ekf_x[3:9]

        self.save_chunk_to_memory(saved_state)
    
    #state array accessors for convenience
    @property
    def state_q_target(self) -> np.ndarray:
        return self.state_array[:,0:4]

    @property
    def state_q_est(self) -> np.ndarray:
        return self.state_array[:,4:8]

    @property
    def state_ake(self) -> np.ndarray:
        return self.state_array[:,8:11]
    
    @property
    def state_ape(self) -> np.ndarray:
        return self.state_array[:,11:14]

    @property
    def state_ake_abs(self) -> np.ndarray:
        return self.state_array[:,14]

    @property
    def state_ape_abs(self) -> np.ndarray:
        return self.state_array[:,15]

    @property
    def state_rate_est(self) -> np.ndarray:
        return self.state_array[:,16:19]

    @property
    def state_mode(self) -> np.ndarray:
        return self.state_array[:,19]

    @property
    def state_mode(self) -> np.ndarray:
        return self.state_array[:,19]

    @property
    def state_t_mtq(self) -> np.ndarray:
        return self.state_array[:,20:23]
    
    @property
    def state_ekf_rate(self) -> np.ndarray:
        return self.state_array[:,23:26]

    @property
    def state_ekf_q(self) -> np.ndarray:
        return self.state_array[:,26:30]

    @property
    def state_ekf_bias(self) -> np.ndarray:
        return self.state_array[:,30:33]
