from typing import Iterable
import pathlib
from datetime import datetime

from alea.obcfw.telem import telem_spec
from alea.obcfw.util import codegen, spec_utils

class TelemCodeGen(codegen.CodeGenerator):
    def __init__(self, output_dir: str | pathlib.Path):
        super().__init__(pathlib.Path(__file__).parent / "templates", output_dir)

    def generate(self, specs_paths: Iterable[str] | Iterable[pathlib.Path] = None) -> tuple[pathlib.Path, pathlib.Path]:
        # Load telem specs
        telem_spec_data = None
        if specs_paths is not None:
            telem_spec_data = spec_utils.load_json_files(specs_paths)
        telem_specs = telem_spec.OBCTelemSpecs.load(telem_spec_data)
        print(f"Loaded {telem_specs.count} telem specifications")

        now = datetime.now()

        # Generate header file
        header_path = self.generate_file(
            "telem_gen.h",
            timestamp = now,
            telem_specs = telem_specs
        )

        # Generate source file
        source_path = self.generate_file(
            "telem_gen.c",
            timestamp          = now,
            telem_gen_header = header_path.name,
            telem_specs          = telem_specs,
        )

        return (header_path, source_path)
