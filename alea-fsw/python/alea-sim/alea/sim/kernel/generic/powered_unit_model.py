from enum import Enum, IntEnum
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.kernel.scheduler import EventPriority
from abc import abstractmethod

class PoweredUnitModel(AbstractModel):
    """
    Simple powered unit that turns on and off and consumes power
    The power calculation is DC power (resisitve loads).
    Alternatively the load can be resistive and only have a constant power.
    """
    class PoweredState(IntEnum):
        NOT_POWERED_ON = 0
        POWERED_ON = 1

    def __init__(self, name:str, sim_kernel:AleasimKernel) -> None:
        super().__init__(name, sim_kernel)
        self._energy_consumed: float = 0.0
        self._powered_state: PoweredUnitModel.PoweredState = PoweredUnitModel.PoweredState.NOT_POWERED_ON
        self._power_on_time: float = -1.0
        
        self.kernel.schedule_event(0, EventPriority.POWER_EVENT, self._power_tick, period=self.kernel.timestep)
    
    @abstractmethod
    def calculate_active_power_usage(self) -> float:
        raise NotImplementedError()
    
    @property
    @abstractmethod
    def current(self) -> float:
        raise NotImplementedError()

    def update_load_power(self, value:float):
        """Watts"""
        self._power_usage = value
        
    def _power_tick(self):
        #assumption: _power_tick updates at a tickrate of self.kernel.timestep
        self._energy_consumed += (self.power_usage * self.kernel.timestep)
        
    @property
    def energy_consumed(self) -> float:
        """Joules"""
        return self._energy_consumed

    @property
    def power_usage(self) -> float:
        """Watts"""
        if self._powered_state == PoweredUnitModel.PoweredState.POWERED_ON:
            return self.calculate_active_power_usage()
        else:
            return 0.0

    @property
    def powered_state(self) -> PoweredState:
        return self._powered_state
    
    @property
    def is_powered_off(self) -> bool:
        return self._powered_state == self.PoweredState.NOT_POWERED_ON
    
    @property
    def uptime(self) -> float:
        """time since power on"""
        if self._power_on_time >= 0:
            return self.kernel.time - self._power_on_time
        else:
            return 0
 
    def power_on(self):
        self._powered_state = PoweredUnitModel.PoweredState.POWERED_ON
        self._power_on_time = self.kernel.time
    
    def power_off(self):
        self._powered_state = PoweredUnitModel.PoweredState.NOT_POWERED_ON
        self._power_on_time = -1