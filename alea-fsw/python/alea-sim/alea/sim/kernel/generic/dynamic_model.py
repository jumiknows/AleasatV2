import abc
import numpy as np

from scipy.integrate import solve_ivp, quad_vec

from .abstract_model import AbstractModel

class DynamicModel:
    def __init__(self, integrator_type: str = 'rk4_inline', **kwargs) -> None:
        super().__init__(**kwargs)
        self._state : np.ndarray = None
        self._state_derivative : np.ndarray = None
        self.integrator_type = integrator_type

    @abc.abstractmethod
    def state_update_fcn(self, t:np.ndarray|float, x:np.ndarray | float, u:np.ndarray | float, update_params: dict = None) -> np.ndarray | float:
        """State update function that returns the state derivative at time t. Must be implemented in inheriting class.

        Args:
            t:
                System time.

            x:
                System state.

            u:
                System input.

            update_params (optional):
                Any additional parameters needed to calculate the state derivative in a dictionary.

        Returns:
            Time derivative of system state.
        """
        raise NotImplementedError
    
    def step_system_dynamics(self, u:np.ndarray, update_params = None):
        """Integrate the state from the LAST simulation time to the CURRENT time"""
        dt = self.kernel.timestep
        t = self.kernel.time
        t0 = t - dt

        if self.integrator_type == 'rk4_inline':
            #fast
            k1 = self.state_update_fcn(t0, self._state, u, update_params)
            k2 = self.state_update_fcn(t0+dt/2.0, self._state + dt*k1/2.0, u, update_params)
            k3 = self.state_update_fcn(t0+dt/2.0, self._state + dt*k2/2.0, u, update_params)
            k4 = self.state_update_fcn(t0+dt, self._state + dt*k3, u, update_params)

            self._state_derivative = 1.0/6.0 * (k1 + 2.0*k2 + 2.0*k3 + k4)
            self._state += dt * self._state_derivative
        elif self.integrator_type == 'rk45':
            time_steps = [t0, t]
            res = solve_ivp(self.state_update_fcn, time_steps, self._state, method='RK45', t_eval=[t], dense_output=False, args=(u, update_params))
            self._state = res.y[:,0]
        #a general adaptive integrator, uses FORTRAN routines
        #and also optionally uses multiple processes to parallelize the integration
        elif self.integrator_type == 'quad':
            #TODO explore effect of limit on accuracy and compute time
            #limit represents the number of subsets the adaptive integrator takes
            #TODO enable multiprocessing for performance improvement
            y, err = quad_vec(self.state_update_fcn, a=t0, b=t, args=(self._state, u, update_params), limit=10000)
            self._state += y
        elif self.integrator_type == 'euler':
            self._state_derivative = self.state_update_fcn(t0, self._state, u, update_params)
            self._state += self._state_derivative * dt