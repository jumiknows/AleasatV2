import numpy as np
import dataclasses
from pathlib import Path

from scipy.signal import lti, lsim

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.configuration import Configurable
from alea.sim.kernel.time_cached import TimeCachedModel, time_cached_property
from alea.sim.kernel.kernel import SharedMemoryModelInterface
from alea.sim.kernel.scheduler import EventPriority
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.kernel.generic.dynamic_model import DynamicModel
from alea.sim.kernel.generic.powered_unit_model import PoweredUnitModel
from alea.sim.kernel.frames import ReferenceFrame, FrameTransformation
from alea.sim.math_lib import Quaternion

@dataclasses.dataclass
class ReactionWheelConfig:
    """Reaction wheel configuration dataclass"""

    @dataclasses.dataclass(frozen=True)
    class ReactionWheelTFCoeffs:
        """
        Reaction wheel transfer function configuration
        This is a second order transfer function of the form:

                                Tz*s + 1                accel(s)
            G(s) = Kp * -------------------------- = ---------------
                        Tw**2*s + 2*Tw*Zeta*s + 1     accel_cmd(s)
        """
        Kp      : float
        Tw      : float
        Zeta    : float
        Td      : float
        Tz      : float

    enable_momentum_saturation: bool
    use_tf              : bool
    tf_coeffs           : ReactionWheelTFCoeffs
    wheel_inertia       : float
    power_saturation    : float
    torque_saturation   : float
    voltage             : float
    motor_constant      : float
    coulomb_friction    : float
    viscous_friction    : float
    max_speed_rads      : float
    Us                  : float
    Ud                  : float
    Cm                  : list

    def __post_init__(self):
        #convert dict[str:float] into ReactionWheelTFCoeffs dataclass
        self.tf_coeffs = self.ReactionWheelTFCoeffs(**self.tf_coeffs)

class ReactionWheelModel(Configurable[ReactionWheelConfig], SharedMemoryModelInterface, PoweredUnitModel, DynamicModel, AbstractModel):
    """
    Reaction Wheel Model.
    
    Initialize with spin_axis_body which is the spin axis in the spacecraft BODY frame.
    """
    
    def __init__(self, sim_kernel: AleasimKernel, name: str, spin_axis_body: np.ndarray, coordinates: np.ndarray = None, cfg: str | Path | dict | ReactionWheelConfig = "reaction_wheel") -> None:
        super().__init__(name=name, sim_kernel=sim_kernel, cfg=cfg, cfg_cls=ReactionWheelConfig)
        self.spin_axis_body = spin_axis_body / np.linalg.norm(spin_axis_body)
        if coordinates is not None:
            self.coordinates = coordinates
            self.logger.info(f"{self.name} is connected with coordinates {self.coordinates}")
        else:
            self.coordinates = np.array(self.cfg.Cm)
            self.logger.warning(f"Could not find the coordinates for the reaction wheel named {self.name}, using the CoM for the coordinates instead")
        self.configure()

    # ==============================================================================
    # Configuration
    # ==============================================================================
        
    def configure(self):
        tcfg = self.cfg.tf_coeffs
        self.dead_time = tcfg.Td
        self.dead_time_n = round(tcfg.Td / self.kernel.timestep)
        numerator = tcfg.Kp * np.array([tcfg.Tz, 1])
        denominator = tcfg.Kp * np.array([tcfg.Tw ** 2, 2 * tcfg.Zeta * tcfg.Tw, 1])
        self.tf: lti = lti(numerator, denominator)

        self._last_lsim_time = self.kernel.time
        self._last_lsim_input = 0.0
        self._torque_command = 0.0
        self._torque_cmd_no_delay = 0.0

        self._state = np.zeros(2) # ang velocity, rotation angle
        self._state_derivative = np.zeros(2) # ang accel, ang velocity
        self._rxn_torque = 0.0

        R_body_stator = self._find_basis(self.spin_axis_body)
        self._basis = R_body_stator

        # This is not the CoM for the reaction wheel, but the satellite's CoM
        self._sat_CoM = np.array(self.cfg.Cm)
        self._lever_arm = self.coordinates - self._sat_CoM

        # define our static/stator frame
        self.static_frame = self.kernel.universe.create_frame(f"{self.name}_static_frame", 
                                                              self.kernel.body_frame, 
                                                              FrameTransformation(R_body_stator,self._lever_arm))
        
        # initially the spin frame is equal to the static frame
        self.spin_frame = self.kernel.universe.create_frame(f"{self.name}_spin_frame", 
                                                            self.static_frame, 
                                                            FrameTransformation(np.eye(3),self._lever_arm))

    # ==============================================================================
    # Kernel Events
    # ==============================================================================

    def start(self):
        self.kernel.schedule_event(0, EventPriority.HARDWARE_EVENT, self.update, self.kernel.timestep)

    # ==============================================================================
    # Simulation Variables
    # ==============================================================================

    @property
    def torque_command(self) -> float:
        """Reaction wheel torque command [Nm]"""
        return self._torque_command

    @property
    def angular_velocity(self) -> float:
        """Reaction wheel angular velocity about spin axis [rad/s]"""
        return self._state[0]
    
    @property
    def angular_momentum(self) -> float:
        """Reaction wheel angular momentum about spin axis [kg * m^2 * rad/s]"""
        return self._state[0] * self.cfg.wheel_inertia

    @property
    def angular_accel(self) -> float:
        """Reaction wheel angular accel about spin axis [rad/s^2]"""
        return self._state_derivative[0]
    
    @property
    def rotation_angle(self) -> float:
        """Reaction wheel rotation angle [rad]"""
        return self._state[1]

    @property
    def torque(self) -> float:
        """Reaction wheel net torque about spin axis as applied to spacecraft body (including friction) [Nm]"""
        return self._rxn_torque

    @property
    def torque_friction(self) -> float:
        """Reaction wheel friction torque about spin axis [Nm]"""
        return self._torque_friction
    
    @property
    def current(self) -> float:
        """Reaction wheel current draw [A]"""
        return np.abs(self.torque / self.cfg.motor_constant)
    
    @property
    def torque_vector(self) -> float:
        """RW torque vector [Nm] in body frame"""
        return self.torque * self.spin_axis_body

    @property
    def angular_momentum_vector(self) -> float:
        """RW angular momentum [kg * m^2 * rad/s] vector in body frame"""
        return self.angular_momentum * self.spin_axis_body

    @property
    def dynamic_torque(self) -> np.ndarray:
        """RW dynamic imbalance torque in body frame [Nm]"""
        tau_d = np.array([self.cfg.Ud*(self.angular_velocity**2),0,0])
        torque_in_body_frame = self._calculate_rotation_matrix() @ tau_d
        return torque_in_body_frame
    
    @property
    def static_torque(self) -> np.ndarray:
        """RW static imbalance torque in body frame [Nm]"""
        force_in_body_frame = self.static_force
        
        return np.cross(self._lever_arm, force_in_body_frame)
    
    @property
    def static_force(self) -> np.ndarray:
        """RW static imbalance force in body frame [N]"""
        force = np.array([self.cfg.Us*(self.angular_velocity**2),0,0])
        force_in_body_frame = self._calculate_rotation_matrix() @ force
        return force_in_body_frame

    # ==============================================================================
    # State Update
    # ==============================================================================
    
    def set_torque_command(self, val: float):
        """Send RW torque command in Nm at current simulation time,
        will account for effect of communication delay."""
        if self.is_powered_on:
            self._torque_cmd_no_delay = val
            self.kernel.schedule_event(self.dead_time, EventPriority.HARDWARE_EVENT, self._set_torque_command, argument=(val, ))

    def _calculate_rotation_matrix(self) -> np.ndarray:
        """Returns the transformation between spin frame and body frame"""
        R_spin_body = self.spin_frame.get_transformation_to(self.kernel.body_frame).rotation
        return R_spin_body
    
    @staticmethod
    def _find_basis(z_stator_body: np.ndarray) -> np.ndarray:
        """Solves for the set of orthonormal basis vectors defining the reaction wheel frame using the Gram-Schmidt process"""
        # Normalizing z_stator_body
        z_stator_body = z_stator_body / np.linalg.norm(z_stator_body)
        # Two candidate vectors to use in the process, z_stator_body cannot be collinear to both
        v1 = np.array([0,0,1])
        v2 = np.array([1,0,0])
        if np.linalg.norm(np.cross(z_stator_body,v1)) == 0:
            y_stator_body = v2 - np.dot(v2,z_stator_body)*z_stator_body
            y_stator_body = y_stator_body / np.linalg.norm(y_stator_body)
            x_stator_body = np.cross(z_stator_body,y_stator_body)
        else:
            y_stator_body = v1 - np.dot(v1,z_stator_body)*z_stator_body
            y_stator_body = y_stator_body / np.linalg.norm(y_stator_body)
            x_stator_body = np.cross(z_stator_body,y_stator_body)
        R_body_stator = np.stack((x_stator_body,y_stator_body,z_stator_body))
        return R_body_stator

    def _set_torque_command(self, val: float):
        self._torque_command = val

    def _on_power_off(self) -> None:
        self._torque_command = 0.0
        self._torque_cmd_no_delay = 0.0
        

    def state_update_fcn(self, t:float, x:float, u:float, update_params: dict = None) -> np.ndarray | float:
        """
        State update function, reaction wheel state is angular velocity.
        Input is the desired torque command.
        
        Returns the angular acceleration of the reaction wheel.
        """

        # T_wheel = J * ang_accel + T_friction
        # https://hanspeterschaub.info/basilisk/_downloads/17eeb82a3f1a8e0b8617c8b8284303ed/Basilisk-REACTIONWHEELSTATEEFFECTOR-20170816.pdf
        self._torque_friction = -1.0 * (self.cfg.coulomb_friction * np.sign(x[0]) + self.cfg.viscous_friction * x[0])
        if (self.is_powered_off or u == 0.0):
            if np.abs(x[0]) > 1e-10:
                wheel_accel = self._torque_friction / self.cfg.wheel_inertia
                self._rxn_torque = -1.0*(self._torque_friction)
            else:
                wheel_accel = 0.0
                self._rxn_torque = 0.0
        else: 
            # calculate and clamp desired wheel motor torque to achieve body torque u by cancelling out friction
            motor_torque = u - self._torque_friction
            motor_torque = min(max(motor_torque, -self.cfg.torque_saturation), self.cfg.torque_saturation)

            #calculate acceleration
            wheel_accel = -1.0*(motor_torque + self._torque_friction) / self.cfg.wheel_inertia

            # TODO ALEA-2387 support reaction wheel transfer function in reaction wheel dynamics update
            # calling this previously raised array shape errors with scipy's lsim function
            # if self.cfg.use_tf:
            #     # reaction wheel acceleration transfer function
            #     # simulate unideal affects, tf parameters are determined by physics testing of the RW from WITTENSTEIN (refer to config)
            #     _, y, _ = lsim(self.tf, np.array([self._last_lsim_input, wheel_accel]), np.array([self._last_lsim_time, t]), self._state_derivative)
            #     self._last_lsim_input = wheel_accel
            #     self._last_lsim_time = t
            #     wheel_accel = y[-1]

            self._rxn_torque = motor_torque + self._torque_friction

        return np.array([wheel_accel, self.angular_velocity])
        
    
    def update(self):
        """call at update rate of orbital dynamics"""
        
        self.step_system_dynamics(self._torque_command)

        self.spin_frame.rotate_by_axang(axis=np.array([0,0,1.0]), angle=self.rotation_angle, relative_to=f"{self.name}_static_frame")
        

        if self.cfg.enable_momentum_saturation and self.is_powered_on and \
            np.abs(self.angular_velocity) > self.cfg.max_speed_rads:
                self.logger.warning(f"exceeded wheel speed limit of {self.cfg.max_speed_rads} [rad/s]")
                self.logger.warning("powering off")
                self.power_off()

        self.save_chunk_to_memory([self.torque,
                                   self._torque_command,
                                   self._torque_cmd_no_delay,
                                   self.angular_accel,
                                   self._torque_friction,
                                   self.angular_velocity,
                                   self.angular_momentum,
                                   self.current,
                                   self.calculate_active_power_usage(),
                                   self.rotation_angle,
                                   np.linalg.norm(self.static_torque),
                                   np.linalg.norm(self.dynamic_torque)])
        

    def calculate_active_power_usage(self) -> float:
        return self.cfg.voltage * self.current

    @property
    def saved_state_element_names(self):
        if not hasattr(self, '_saved_state_element_names'):
            self._saved_state_element_names = ['torque_rw',
                                               'torque_cmd_rw',
                                               "torque_cmd_no_delay_rw",
                                               'acceleration_rw',
                                               'friction_torque_rw',
                                               'velocity_rw',
                                               'momentum_rw',
                                               'current_rw',
                                               'power_rw',
                                               'angle_rw',
                                               'static_imbalance_rw',
                                               'dynamic_imbalance_rw']
        return self._saved_state_element_names

    @property
    def saved_state_size(self):
        return len(self.saved_state_element_names)