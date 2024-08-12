import abc
import numpy as np

from scipy.integrate import solve_ivp, quad_vec

from .abstract_model import AbstractModel

class DynamicModel(AbstractModel):
    def __init__(self, name: str, sim_kernel: AbstractModel, integrator_type: str = 'rk45') -> None:
        super().__init__(name, sim_kernel)
        self._state : np.ndarray = None
        self._state_derivative : np.ndarray = None
        self._integrator_type = integrator_type
    
    @property
    def integrator_type(self) -> str:
        return self._integrator_type

    @integrator_type.setter
    def integrator_type(self, value:str):
        self._integrator_type = value
    
    @abc.abstractmethod
    def state_update_fcn(self, t:np.ndarray|float, x:np.ndarray, u:np.ndarray, update_params: dict = None) -> np.ndarray:
        """
        State update function that returns the state derivative at time t. Must be implemented in inheriting class.

        Parameters
        ----------
        x : array
            System state
        u : array
            System input
        update_params: dictionary
            Any additional parameters needed to calculate the state derivative in a dictionary

        Returns
        -------
        dx : array
            time deterivative of system state
        """
        raise NotImplementedError
    
    def step_system_dynamics(self, u:np.ndarray, update_params = None):
        """Integrate the state from the LAST simulation time to the CURRENT time"""
        dt = self.kernel.timestep
        t = self.kernel.time
        t0 = t - dt

        if self._integrator_type == 'rk45':
            time_steps = [t0, t]
            res = solve_ivp(self.state_update_fcn, time_steps, self._state, method='RK45', t_eval=[t], dense_output=False, args=(u, update_params))
            self._state = res.y[:,0]
        #a general adaptive integrator, uses FORTRAN routines
        #and also optionally uses multiple processes to parallelize the integration
        elif self._integrator_type == 'quad':
            #TODO explore effect of limit on accuracy and compute time
            #limit represents the number of subsets the adaptive integrator takes
            #TODO enable multiprocessing for performance improvement
            y, err = quad_vec(self.state_update_fcn, a=t0, b=t, args=(self._state, u, update_params), limit=10000)
            self._state += y
        elif self._integrator_type == 'euler':
            self._state_derivative = self.state_update_fcn(t0, self._state, u, update_params)
            self._state += self._state_derivative * dt