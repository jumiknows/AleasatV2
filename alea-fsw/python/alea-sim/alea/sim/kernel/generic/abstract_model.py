from typing import Set
import abc
import weakref

import logging

from alea.sim.kernel.scheduler import Scheduler
from alea.sim.kernel.config_loader import load_config

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from alea.sim.kernel.kernel import AleasimKernel

class ModelNotFoundError(Exception):
    pass

class AbstractModel(abc.ABC):
    """
    Abstract model tree.
    """
    def __init__(self, name: str, sim_kernel: "AleasimKernel", params: dict = None, parent: "AbstractModel" = None, children: dict[str, "AbstractModel"] = None, **kwargs) -> None:
        super().__init__(**kwargs)

        self._name = name
        self._kernel: "AleasimKernel" = sim_kernel

        if params is None:
            params = {}
        self._params = params

        if parent is not None:
            self._parent: AbstractModel = weakref.proxy(parent)
        else:
            self._parent = None

        if children is None or type(children) is not dict:
            self._children: dict[str, AbstractModel] = {}
        else:
            self._children = children

        if not hasattr(self, '_logger'):
            self._logger = sim_kernel.logger.getChild(name)

        if not hasattr(self, '_scheduler'):
            self._scheduler = sim_kernel.scheduler

    @property
    def config_name(self) -> str:
        # TODO: Remove this and use Config classes instead
        raise NotImplementedError

    def connect(self):
        """
        Connect models to each other, and obtain references to attributes of other models
        this function may return an error if the parent is undefined or if a model is missing
        from the tree.
        
        This method will be invoked as a kernel event at simulation time 0 before any other events
        on this model.
        """
        pass

    def start(self):
        """
        Start any processes that require the connections established in self.connect() and must also
        start before the main simulation.
        
        This method will be invoked as a kernel event at simulation time 0 after all connect events
        but before any other events.
        """
        pass

    @property
    def logger(self) -> logging.Logger:
        """A logger object (can be a class attribute) to log messages to."""
        return self._logger

    @property
    def name(self) -> str:
        return self._name

    @property
    def kernel(self) -> "AleasimKernel":
        return self._kernel

    @property
    def params(self) -> dict:
        return self._params

    @property
    def parent_model(self) -> "AbstractModel":
        return self._parent

    @property
    def children_models(self) -> Set[str]:
        return self._children.keys()
    
    @property
    def scheduler(self) -> Scheduler:
        return self._scheduler

    def get_child(self, name_or_type: str | type, is_base: bool = True) -> "AbstractModel":
        """
        Traverse the tree opting for depth first to find children model.
        
        Find child model either based on name (unique) or class type (not unique).
        For matching class type, get_child will return the first match found.
        
        is_base tells get_child whether it is the base caller or not for the recursion.
        if it is the base caller, an error is raised when models are not found, otherwise none is returned upwards.
        """
        if type(name_or_type) is str:
            search_mode = 'name'
        elif issubclass(name_or_type, AbstractModel):
            search_mode = 'class'
        else:
            search_mode = 'undefined'

        if not self._children:
            if is_base:
                raise  ModelNotFoundError(f'child model {name_or_type} could not be found from {self.name}')
            else: 
                return None

        if search_mode == 'class':
            cls = name_or_type
            for child in self._children.values():
                if isinstance(child, cls):
                    return child
            for child in self._children.values():
                val = child.get_child(cls, False)
                if val is not None:
                    return val
            if is_base:
                raise ModelNotFoundError(f'child model of type {cls} could not be found from {self.name}')
            else: 
                return None
        elif search_mode == 'name':
            name = name_or_type
            if name in self._children.keys():
                return self._children[name]
            else:
                for child in self._children.values():
                    val = child.get_child(name, False)
                    if val is not None:
                        return val
            if is_base:
                raise ModelNotFoundError(f'child model of type {name} could not be found from {self.name}')
            else: 
                return None
        else:
            raise Exception(f'search mode is invalid: {search_mode}')

    def get_all_children_of_type(self, cls: type) -> list["AbstractModel"]:
        """
        Traverse the tree opting for depth first to find children model of same class.
        Returns an empty list if no models of class found.
        
        is_base tells get_child whether it is the base caller or not for the recursion.
        """
        if not issubclass(cls, AbstractModel):
            raise Exception(f'{cls} is not a subclass of AbstractModel')

        out_list: list[AbstractModel] = []

        if not self._children:
            return out_list

        for child in self._children.values():
            if isinstance(child, cls):
                out_list.append(child)
            val = child.get_all_children_of_type(cls)
            if len(val) > 0:
                out_list.extend(val)
        
        return out_list

    def add_child(self, child: "AbstractModel") -> None:
        self._children[child.name] = child
        child._parent = weakref.proxy(self)
        self.logger.info(f'Child model: {child.name} added to Parent model: {self.name}')
        
    def remove_child(self, child: "AbstractModel"):
        if child.name in self._children.keys():
            self._children.pop(child.name)
            self.logger.info(f'Removed child model: {child.name} from Parent model: {self.name}')
    
    def get_config(self, override_cfg_name:str = None) -> dict:
        if override_cfg_name is None:
            return load_config(self.config_name)
        else:
            return load_config(override_cfg_name)
    
    def propagate_log_level(self, log_level:int):
        """set log level for current model and propagate for all children"""
        self.logger.info(f'Setting log level to {log_level}')
        self.logger.setLevel(log_level)
        
        for child in self._children.values():
            child.propagate_log_level(log_level)
