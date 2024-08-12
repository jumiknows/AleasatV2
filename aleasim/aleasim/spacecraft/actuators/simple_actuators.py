from aleasim.kernel.generic.abstract_model import AbstractModel

class SimpleActuator(AbstractModel):
    """
    Simple actuator that clamps a commanded torque
    """
    
    def __init__(self, name: str, sim_kernel: AbstractModel, sarturation_torque: float):
        super().__init__(name, sim_kernel)
        self._saturation_torque = sarturation_torque
        self._torque = 0
    
    def set_torque(self, val: float):
        """set torque [nM]"""
        self._torque = min(max(val, -self._saturation_torque), self._saturation_torque)

    def get_torque(self) -> float:
        """get torque [nM]"""
        return self._torque
    
    @property
    def config_name(self) -> str:
        return 'simple_actuator'
    
    def connect(self):
        pass
    
class SimpleMagnetorquer(AbstractModel):
    """
    Simple magnetorquer that clamps a commanded dipole moment with certain body frame axis.
    """
    
    def __init__(self, name: str, sim_kernel: AbstractModel, sarturation_moment: float):
        super().__init__(name, sim_kernel)
        self._saturation_moment = sarturation_moment
        self._moment = 0

    def connect(self):
        pass
    
    def set_moment(self, val: float):
        """set moment [Am^2]"""
        self._moment = min(max(val, -self._saturation_moment), self._saturation_moment)

    def get_moment(self) -> float:
        """get moment [Am^2]"""
        return self._moment
    
    @property
    def config_name(self) -> str:
        return 'simple_magnetorquer'