from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.kernel.kernel import SharedMemoryModelInterface
from alea.sim.kernel.scheduler import EventPriority

class SimpleActuator(AbstractModel):
    """
    Simple actuator that clamps a commanded torque
    """
    
    def __init__(self, name: str, sim_kernel: AbstractModel, saturation_torque: float):
        super().__init__(name, sim_kernel)
        self._saturation_torque = saturation_torque
        self._torque = 0
    
    def set_torque(self, val: float):
        """set torque [nM]"""
        self._torque = min(max(val, -self._saturation_torque), self._saturation_torque)

    def get_torque(self) -> float:
        """get torque [nM]"""
        return self._torque
    
class SimpleMagnetorquer(SharedMemoryModelInterface, AbstractModel):
    """
    Simple magnetorquer that clamps a commanded dipole moment with certain body frame axis.
    """
    
    def __init__(self, name: str, sim_kernel: AbstractModel, saturation_moment: float):
        super().__init__(name, sim_kernel)
        self._saturation_moment = saturation_moment
        self._moment = 0
        self._update_period = 0.1

    @property
    def saved_state_size(self) -> int:
        return 1

    @property
    def saved_state_element_names(self) -> list[str]:
        return ['moment']
        
    def start(self):
        self.kernel.schedule_event(0, EventPriority.HARDWARE_EVENT, self.save_state, self._update_period)
    
    def save_state(self):
        self.save_chunk_to_memory(self._moment)
    
    def set_moment(self, val: float):
        """set moment [Am^2]"""
        self._moment = min(max(val, -self._saturation_moment), self._saturation_moment)

    def get_moment(self) -> float:
        """get moment [Am^2]"""
        return self._moment