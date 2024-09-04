from typing import TypeVar, Generic
from pathlib import Path

import pyjson5

C = TypeVar("C")

def get_config(config_name: str) -> Path:
    return Path(__file__).parent / f"{config_name}.cfg"

class Configurable(Generic[C]):
    def __init__(self, cfg: str | Path | dict | C = None, cfg_cls: type[C] = dict, **kwargs):
        super().__init__(**kwargs)
        
        if cfg is not None:
            if isinstance(cfg, str):
                cfg = get_config(cfg)

            if isinstance(cfg, Path):
                with open(cfg) as file:
                    cfg = pyjson5.load(file)
                    assert type(cfg) == dict

            if isinstance(cfg, dict):
                cfg = cfg_cls(**cfg)

            if isinstance(cfg, cfg_cls):
                self._cfg = cfg

    @property
    def cfg(self) -> C:
        return self._cfg
