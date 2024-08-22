from .generic.abstract_model import AbstractModel

class AleasimRootModel(AbstractModel):
    """
    Root model of aleasim.
    """
    root_name = 'aleasim'

    def __init__(self, kernel) -> None:
        super().__init__(self.root_name, kernel)

    def connect(self):
        pass

    @property
    def config_name(self) -> str:
        return 'aleasim'