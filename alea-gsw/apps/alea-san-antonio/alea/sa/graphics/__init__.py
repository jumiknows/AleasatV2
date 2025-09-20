from importlib.resources import files, as_file
import contextlib
from pathlib import Path
import sys

def get_path(filename) -> contextlib.AbstractContextManager[Path, bool | None]:
    return as_file(files(sys.modules[__name__]).joinpath(filename))
