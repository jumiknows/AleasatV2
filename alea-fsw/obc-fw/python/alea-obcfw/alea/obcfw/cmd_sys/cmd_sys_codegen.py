from typing import Iterable
import pathlib
from datetime import datetime

from alea.obcfw.cmd_sys import cmd_sys_spec
from alea.obcfw.util import codegen, spec_utils

class CmdSysCodeGen(codegen.CodeGenerator):
    def __init__(self, output_dir: str | pathlib.Path):
        super().__init__(pathlib.Path(__file__).parent / "templates", output_dir)

    def generate(self, specs_paths: Iterable[str] | Iterable[pathlib.Path] = None) -> tuple[pathlib.Path, pathlib.Path]:
        # Load command specs
        cmd_spec_data = None
        if specs_paths is not None:
            cmd_spec_data = spec_utils.load_json_files(specs_paths)
        cmd_specs = cmd_sys_spec.OBCCmdSysSpecs.load(cmd_spec_data)
        print(f"Loaded {cmd_specs.count} command specifications")

        now = datetime.now()

        # Generate header file
        header_path = self.generate_file(
            "cmd_sys_gen.h",
            timestamp = now,
            cmd_specs = cmd_specs
        )

        # Generate source file
        source_path = self.generate_file(
            "cmd_sys_gen.c",
            timestamp          = now,
            cmd_sys_gen_header = header_path.name,
            cmd_specs          = cmd_specs,
        )

        return (header_path, source_path)
