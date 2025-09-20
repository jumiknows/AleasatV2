from typing import Iterable
import pathlib
from datetime import datetime

from alea.obcfw.log import log_spec
from alea.obcfw.util import codegen, spec_utils

class LogCodeGen(codegen.CodeGenerator):
    def __init__(self, output_dir: str | pathlib.Path):
        super().__init__(pathlib.Path(__file__).parent / "templates", output_dir)

    def generate(self, specs_paths: Iterable[str] | Iterable[pathlib.Path] = None) -> pathlib.Path:
        # Load log specs
        log_spec_data = None
        if specs_paths is not None:
            log_spec_data = spec_utils.load_json_files(specs_paths)
        log_specs = log_spec.OBCLogGroupSpecs.load(log_spec_data)
        print(f"Loaded {log_specs.count} log group specifications")

        now = datetime.now()

        # Generate header file
        header_path = self.generate_file(
            "logger.h",
            timestamp = now,
            log_specs = log_specs,
        )

        return header_path
