import numpy as np
from dataclasses import dataclass
from pathlib import Path

from alea.sim.configuration import Configurable
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.math_lib.math import skew, normalize
from alea.sim.kernel.kernel import SharedMemoryModelInterface
from alea.sim.kernel.generic.dynamic_model import DynamicModel
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.math_lib import Quaternion
from alea.sim.kernel.scheduler import EventPriority
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.epa.disturbance_model import DisturbanceModel
from alea.sim.kernel.generic.abstract_model import ModelNotFoundError

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from alea.sim.spacecraft.aocs import AOCSModel
    from alea.sim.spacecraft.spacecraft import Spacecraft
    
@dataclass
class AttitudeDynamicsConfig:
    """
    Attitude Dyanmics config dataclass
    """
    default_state       : np.ndarray
    J                   : np.ndarray
    J_inv               : np.ndarray = None
    enable_disturbances : bool = True

    def __post_init__(self):
        #convert list to np.ndarry
        if isinstance(self.J, list):
            self.J = np.array(self.J)
        if isinstance(self.default_state, list):
            self.default_state = np.array(self.default_state)
        
        self.J_inv = np.linalg.inv(self.J)

class AttitudeDynamicsModel(Configurable[AttitudeDynamicsConfig], SharedMemoryModelInterface, DynamicModel, AbstractModel):
    """
    Model for the attitude dynamics of an inertial pointing spacecraft

    Based on section 4.3.1 of Yang, Yaguang - Spacecraft modeling, attitude determination, 
    and control_ quaternion-based approach
    
    The spacecraft attitude state is is a 7 element vector composed of
    - quaternion elements (4) (rotation of body frame w.r.t inertial frame)
    - body angular rates (3) (angular velocity of body frame w.r.t inertial frame)
    """
    default_name = 'attitude_dynamics'
    def __init__(self, 
                 sim_kernel: AleasimKernel, 
                 init_state:np.ndarray = None, 
                 name = "attitude_dynamics", 
                 cfg: str | Path | dict | AttitudeDynamicsConfig = "attitude_dynamics", 
                 **kwargs
                 ) -> None:
        super().__init__(name=name, sim_kernel=sim_kernel, cfg=cfg, cfg_cls=AttitudeDynamicsConfig, **kwargs)

        if init_state is not None:
            if init_state.size == 7:
                self._state = init_state
            else:
                raise ValueError("init_state is invalid size (should be 7)")
        else:
            self._state = self.cfg.default_state
        self._state_derivative = np.zeros(7)

        self._aocs = None
        self._magm = None
        self._disturbances: DisturbanceModel = None

        self.logger.info(f'initialized with state {self._state}')
        self.logger.info(f'loaded intertia matrix cfg {self.cfg.J} and its inverse is computed to be {self.cfg.J_inv}')

    def connect(self):
        try:
            self._aocs = self.kernel.get_model('aocs')
        except ModelNotFoundError as err:
            self.logger.warning(err)
            self._aocs: AOCSModel = None

        try:
            self._magm: EarthMagneticFieldModel = self.kernel.get_model(EarthMagneticFieldModel)
        except ModelNotFoundError as err:
            self.logger.warning(err)
            self._magm = None
            
        try:
            self._disturbances: DisturbanceModel = self.kernel.get_model(DisturbanceModel)
        except ModelNotFoundError as err:
            self.logger.warning(err)
            self._disturbances: DisturbanceModel = None
        
        self.kernel.schedule_event(self.kernel.timestep, EventPriority.ATTITUDE_DYNAMICS_EVENT, self.update, period=self.kernel.timestep)

    @property
    def saved_state_size(self) -> int:
        #state vector (7) + ang accel (3) + rxn wheel imbalance torques (3) + disturbance torque (3)
        return (self._state.size + 3 + 3 + 3)

    @property
    def saved_state_element_names(self) -> list[str]:
        return ['q0','q1','q2','q3','w1','w2','w3','ang_accel1','ang_accel2','ang_accel3','Ldx', 'Ldy', 'Ldz', 'Tdx', 'Tdy', 'Tdz']

    @property
    def quaternion(self) -> Quaternion:
        """rotation relative to inertial frame"""
        return Quaternion(self._state[0:4])
    
    @property
    def quaternion_derivative(self) -> np.ndarray:
        return self._state_derivative[0:4]

    @property
    def angular_velocity(self) -> np.ndarray:
        """rad/s"""
        return self._state[4:7]

    @property
    def angular_acceleration(self) -> np.ndarray:
        """rad/s^2"""
        return self._state_derivative[4:7]

    def set_state(self, value: np.ndarray):
        if value.shape != self._state.shape or value.dtype != self._state.dtype:
            raise ValueError('shapes or datatypes do not match')
        
        self._state = value.copy()
        self._state_derivative = np.zeros(self._state.shape)
    
    def calculate_rotational_kinetic_energy(self) -> float:
        """K= 0.5 * Iω2  [Joules]"""
        omega = self._state[4:7]
        ke:float = (0.5 * ((self.cfg.J @ omega) @ omega))
        return ke
    
    def update(self):
        #set current state of reaction wheels
        saved_state = np.zeros(self.saved_state_size)
        update_params = {}
        update_params['h_wheels'] = np.zeros(3)
        
        actuator_torque = np.zeros(3)
        if self._aocs is not None and self._magm is not None:
            #actuator torques
            #torque output from rws and mtqs at this time instance
            aocs = self._aocs
            mtq_moments = np.zeros(3)
            for i in range(3):
                actuator_torque += aocs._rws[i].torque_vector
                update_params['h_wheels'] += aocs._rws[i].angular_momentum_vector
                mtq_moments += aocs._mtqs[i].get_moment()

            b_body = self._magm.mag_field_vector_body * 1E-9
            t_mtq_body = np.cross(mtq_moments, b_body)
            actuator_torque += t_mtq_body

        #disturbance torques
        #torques from gravity gradient, magnetic, aerodynamic disturbances, reaction wheel imbalances
        dynamic_imbalance = np.zeros(3)
        static_imbalance = np.zeros(3)
        rw_imbalances = np.zeros(3)
        disturbance_torque = np.zeros(3)
        if self._disturbances is not None and self.cfg.enable_disturbances:
            if self._aocs is not None:
                for wheel in self._aocs._rws:
                    dynamic_imbalance += wheel.dynamic_torque
                    static_imbalance += wheel.static_torque
                rw_imbalances = static_imbalance + dynamic_imbalance
                disturbance_torque += rw_imbalances
            saved_state[10:13] = rw_imbalances

            disturbance_torque += self._disturbances.get_T_disturbance()
            saved_state[13:] = disturbance_torque

        u = actuator_torque + disturbance_torque

        self.step_system_dynamics(u, update_params)
        
        self._state[0:4] = normalize(self._state[0:4])
        saved_state[0:7] = self._state
        saved_state[7:10] = self.angular_acceleration
        self.save_chunk_to_memory(saved_state)
        
    def state_update_fcn(self, t:np.ndarray, x:np.ndarray, u:np.ndarray, update_params: dict = None) -> np.ndarray:
        """
        Parameters
        ----------
        x : array
            System state: [q0, q1, q2, q3, omega1, omega2, omega3]
        u : array
            System input (actuator + disturbance torque): [u1, u2, u3]

        Returns
        -------
        dx : array
            time deterivative of system state
        """
        
        #dynamic parameters
        h_wheels = update_params.get('h_wheels', np.zeros(3, dtype=np.float64))
        
        #state vectors
        q = normalize(x[:4]) #quaternion axes at time t
        omega = x[4:7] #angular rates at time t
        
        #dynamics
        accel = self.cfg.J_inv @ (-1.0 * skew(omega)@(self.cfg.J @ omega + h_wheels) + u)
        
        #kinematics
        Q = 0.5*np.array([[q[0], -q[1], -q[2], -q[3]],
                    [q[1], q[0], -q[3], q[2]],
                    [q[2], q[3], q[0], -q[1]],
                    [q[3], -q[2], q[1], q[0]]])
        omega0 = np.zeros(4)
        omega0[1:] = omega
        dq = Q @ omega0
        
        #updated state
        dx = np.zeros(7)
        dx[:4] = dq
        dx[4:7] = accel
        self._state_derivative = dx
        self._t_derivative = t
        
        return dx
