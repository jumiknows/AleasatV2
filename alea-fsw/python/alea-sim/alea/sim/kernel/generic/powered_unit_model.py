from enum import Enum
from .abstract_model import AbstractModel
from ..kernel import AleasimKernel

class PoweredUnitModel(AbstractModel):
    """Simple powered unit that turns on and off and consumes power."""
    class PoweredState(Enum):
        NOT_POWERED_ON = 0
        POWERED_ON = 1

    def __init__(self, name:str, sim_kernel:AleasimKernel, power_usage: float = 0) -> None:
        super().__init__(name, sim_kernel)
        self._power_usage: float = power_usage
        self._powered_state: PoweredUnitModel.PoweredState = PoweredUnitModel.PoweredState.NOT_POWERED_ON
        self._power_on_time: float = -1

    @property
    def nominal_power_usage(self) -> float:
        """power usage when active"""
        return self._power_usage

    @nominal_power_usage.setter
    def set_nominal_power_usage(self, value:float):
        self._power_usage = value
    
    @property
    def current_power_usage(self) -> float:
        if self._powered_state == PoweredUnitModel.PoweredState.POWERED_ON:
            return self._power_usage
        else:
            return 0.0

    @property
    def powered_state(self) -> PoweredState:
        return self._powered_state
    
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