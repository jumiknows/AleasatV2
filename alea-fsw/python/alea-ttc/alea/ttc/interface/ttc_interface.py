from abc import ABC, abstractmethod

class TTCInterface(ABC):
    @property
    @abstractmethod
    def connected(self) -> bool:
        pass

    @abstractmethod
    def start(self, *args, **kwargs) -> bool:
        pass

    @abstractmethod
    def stop(self):
        pass
