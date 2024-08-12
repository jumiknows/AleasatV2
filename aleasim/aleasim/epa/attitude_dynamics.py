import numpy as np

from aleasim.kernel.kernel import AleasimKernel
from aleasim.math_lib.math import skew, normalize
from aleasim.kernel.kernel import SharedMemoryModelInterface
from aleasim.kernel.generic.dynamic_model import DynamicModel
from aleasim.math_lib import Quaternion
from aleasim.kernel.scheduler import EventPriority
from aleasim.epa.magnetic_field_model import EarthMagneticFieldModel
from aleasim.epa.disturbance_model import DisturbanceModel
from aleasim.kernel.generic.abstract_model import ModelNotFoundError

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from aleasim.spacecraft.spacecraft import Spacecraft

class AttitudeDynamicsModel(DynamicModel, SharedMemoryModelInterface):
    """
    Model for the attitude dynamics of an inertial pointing spacecraft

    Based on section 4.3.1 of Yang, Yaguang - Spacecraft modeling, attitude determination, 
    and control_ quaternion-based approach
    
    The spacecraft attitude state is is a 7 element vector composed of
    - quaternion elements (4) (rotation of body frame w.r.t inertial frame)
    - body angular rates (3) (angular velocity of body frame w.r.t inertial frame)
    """
    default_name = 'attitude_dynamics'
    def __init__(self, sim_kernel: AleasimKernel, init_state:np.ndarray = None) -> None:
        super().__init__(AttitudeDynamicsModel.default_name, sim_kernel)
        if init_state is not None and init_state.size == 7:
            self._state = init_state

        self._spacecraft = None
        self._magm = None
        self._disturbances: DisturbanceModel = None

        self.logger.info(f'initialized with state {self._state}')
        self.configure()
    
    def configure(self):
        # self._sys_dynamics = NonlinearIOSystem(self.state_update_fcn, None, name='spacecraft_attitude_dynamics')
        cfg = self.get_config()
        self.params['J'] = cfg['J']#np.eye(3) * 1e-3
        self._integrator_type = cfg['default_integrator']
        
        if self._state is None:
            self._state = np.array(cfg['default_state'])
            self.logger.info(f'loaded default state cfg {cfg["default_state"]}')
        
        #for faster accesss
        self._J = self.params.get('J') #inertia matrix
        self._Jinv = np.linalg.inv(self._J) # inverse inertia for eom state updates
        self.logger.info(f'loaded intertia matrix cfg {self._J} and its inverse is computed to be {self._Jinv}')

        self._state_derivative = np.zeros(7)

    def connect(self):
        try:
            self._spacecraft = self.kernel.get_model('spacecraft')
        except ModelNotFoundError as err:
            self.logger.warn(err)
            self._spacecraft: Spacecraft = None

        try:
            self._magm: EarthMagneticFieldModel = self.kernel.get_model(EarthMagneticFieldModel)
        except ModelNotFoundError as err:
            self.logger.warn(err)
            self._magm = None
            
        try:
            self._disturbances: DisturbanceModel = self.kernel.get_model(DisturbanceModel)
        except ModelNotFoundError as err:
            self.logger.warn(err)
            self._disturbances: DisturbanceModel = None
        
        self.kernel.schedule_event(self.kernel.timestep, EventPriority.DYNAMICS_EVENT, self.update, period=self.kernel.timestep)

    @property
    def config_name(self) -> str:
        return 'attitude_dynamics'
    
    @property
    def saved_state_size(self) -> int:
        #state vector (7) + ang accel (3) + disturbance torque (3)
        return (self._state.size + 3 + 3)

    @property
    def saved_state_element_names(self) -> list[str]:
        return ['q0','q1','q2','q3','w1','w2','w3','ang_accel1','ang_accel2','ang_accel3', 'Tdx', 'Tdy', 'Tdz']

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
        J = self.params.get('J')
        ke:float = (0.5 * ((J @ omega) @ omega))
        return ke
    
    def update(self):
        #set current state of reaction wheels
        saved_state = np.zeros(self.saved_state_size)
        h_wheels = np.zeros(3) #TODO : get reaction wheel state
        update_params = {}
        update_params['wheel1_momentum'] = h_wheels[0]
        update_params['wheel2_momentum'] = h_wheels[1]
        update_params['wheel3_momentum'] = h_wheels[2]
        
        actuator_torque = np.zeros(3)
        if self._spacecraft is not None and self._magm is not None:
            #actuator torques
            #torque output from rws and mtqs at this time instance
            sc = self._spacecraft
            mtq_moments = np.zeros(3)
            for i in range(3):
                actuator_torque[i] += sc._rws[i].get_torque()
                mtq_moments[i] += sc._mtqs[i].get_moment()

            b_body = self._magm.get_mag_vector_body()
            t_mtq_body = np.cross(mtq_moments, b_body)
            actuator_torque += t_mtq_body

        #disturbance torques
        #torques from gravity gradient, magnetic, aerodynamic disturbances
        disturbance_torque = np.zeros(3)
        if self._disturbances is not None:
            disturbance_torque = self._disturbances.get_T_disturbance(self.quaternion)
            disturbance_torque = disturbance_torque.reshape((3,))
            self.logger.debug(f'disturbance torque is {disturbance_torque} nM')
            saved_state[10:] = disturbance_torque

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
        h_wheels = np.zeros(3)
        
        #in case update_params is None
        h_wheels[0] = update_params.get('wheel1_momentum',0)
        h_wheels[1] = update_params.get('wheel2_momentum',0)
        h_wheels[2] = update_params.get('wheel3_momentum',0)         
        
        #state vectors
        q = normalize(x[:4]) #quaternion axes at time t
        omega = x[4:7] #angular rates at time t
        
        #dynamics
        accel = self._Jinv @ (-1.0 * skew(omega)@(self._J@omega + h_wheels) + u)
        
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