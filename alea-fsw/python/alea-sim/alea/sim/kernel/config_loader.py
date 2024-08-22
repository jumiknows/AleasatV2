from pathlib import Path
import logging
import pyjson5

CONFIG_DIR = Path(__file__).parent.parent / "configuration"

def load_config(config_name: str) -> dict:
    """
    Load a config dictionary from .json file in configuration directory.
    json file should follow json5 convention (https://json5.org/)
    """
    path = CONFIG_DIR / f"{config_name}.cfg"
    logging.getLogger('AleasimKernel').info(f'loading model configuration from file {path}')
    
    with open(path) as fp:
        cfg = pyjson5.load(fp)
        assert type(cfg) == dict
        return cfg