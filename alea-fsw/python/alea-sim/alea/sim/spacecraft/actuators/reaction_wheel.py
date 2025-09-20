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

    def __post_init__(self):
        #convert dict[str:float] into ReactionWheelTFCoeffs dataclass
        self.tf_coeffs = self.ReactionWheelTFCoeffs(**self.tf_coeffs)

class ReactionWheelModel(Configurable[ReactionWheelConfig], SharedMemoryModelInterface, PoweredUnitModel, DynamicModel, AbstractModel):
    """
    Reaction Wheel Model.
    
    Initialize with spin_axis_body which is the spin axis in the spacecraft BODY frame.
    """
    
    def __init__(self, sim_kernel: AleasimKernel, name: str, spin_axis_body: np.ndarray,  cfg: str | Path | dict | ReactionWheelConfig = "reaction_wheel") -> None:
        super().__init__(name=name, sim_kernel=sim_kernel, cfg=cfg, cfg_cls=ReactionWheelConfig)
        self.spin_axis_body = spin_axis_body / np.linalg.norm(spin_axis_body)
        self.frame = self.kernel.universe.create_frame(self.name, 
                                                       self.kernel.body_frame, 
                                                       FrameTransformation(Quaternion.from_axang(self.spin_axis_body, 0.0)))
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

        self._state = 0.0 # ang velocity
        self._state_derivative = 0.0 # ang accel
        self._rxn_torque = 0.0

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
        return self._state
    
    @property
    def angular_momentum(self) -> float:
        """Reaction wheel angular momentum about spin axis [kg * m^2 * rad/s]"""
        return self._state * self.cfg.wheel_inertia

    @property
    def angular_accel(self) -> float:
        """Reaction wheel angular accel about spin axis [rad/s^2]"""
        return self._state_derivative

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

    # ==============================================================================
    # State Update
    # ==============================================================================
    
    def set_torque_command(self, val: float):
        """Send RW torque command in Nm at current simulation time,
        will account for effect of communication delay."""
        if self.is_powered_on:
            self._torque_cmd_no_delay = val
            self.kernel.schedule_event(self.dead_time, EventPriority.HARDWARE_EVENT, self._set_torque_command, argument=(val, ))

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
        self._torque_friction = -1.0 * (self.cfg.coulomb_friction * np.sign(x) + self.cfg.viscous_friction * x)
        if (self.is_powered_off or u == 0.0):
            if np.abs(x) > 1e-10:
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

        return wheel_accel
        
    
    def update(self):
        """call at update rate of orbital dynamics"""
        
        self.step_system_dynamics(self._torque_command)
        

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
                                   self.calculate_active_power_usage()])
        

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
                                               'power_rw']
        return self._saved_state_element_names

    @property
    def saved_state_size(self):
        return len(self.saved_state_element_names)