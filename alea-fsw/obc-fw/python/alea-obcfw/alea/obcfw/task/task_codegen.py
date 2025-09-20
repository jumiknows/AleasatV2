from typing import Iterable
import pathlib
from datetime import datetime

from alea.obcfw.task import task_spec
from alea.obcfw.util import codegen, spec_utils

class TaskCodeGen(codegen.CodeGenerator):
    def __init__(self, output_dir: str | pathlib.Path):
        super().__init__(pathlib.Path(__file__).parent / "templates", output_dir)

    def generate(self, specs_paths: Iterable[str] | Iterable[pathlib.Path] = None) -> tuple[pathlib.Path, pathlib.Path, pathlib.Path]:
        # Load task specs
        task_spec_data = None
        if specs_paths is not None:
            task_spec_data = spec_utils.load_json_files(specs_paths)
        task_specs = task_spec.OBCTaskSpecs.load(task_spec_data)
        print(f"Loaded {task_specs.count} task specifications")

        now = datetime.now()

        # Generate ID header file
        id_header_path = self.generate_file(
            "obc_tasks_ids_gen.h",
            timestamp  = now,
            task_specs = task_specs,
        )

        # Generate header file
        header_path = self.generate_file(
            "obc_tasks_gen.h",
            timestamp                = now,
            obc_tasks_ids_gen_header = id_header_path.name,
            task_specs               = task_specs,
        )

        # Generate source file
        source_path = self.generate_file(
            "obc_tasks_gen.c",
            timestamp            = now,
            obc_tasks_gen_header = header_path.name,
            task_specs           = task_specs,
        )

        return (id_header_path, header_path, source_path)
