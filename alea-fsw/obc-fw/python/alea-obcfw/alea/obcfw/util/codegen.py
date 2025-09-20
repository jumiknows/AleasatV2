import abc
from typing import Iterable
import pathlib

import jinja2

class CodeGenerator(abc.ABC):
    def __init__(self, template_dir: str | pathlib.Path, output_dir: str | pathlib.Path):
        self._file_loader = jinja2.FileSystemLoader(template_dir)

        if not isinstance(output_dir, pathlib.Path):
            output_dir = pathlib.Path(output_dir)

        output_dir.mkdir(parents=True, exist_ok=True)
        self._output_dir = output_dir

    def _load_template(self, target: str) -> jinja2.Template:
        env = jinja2.Environment(
            loader=self._file_loader,
            keep_trailing_newline=True
        )
        template = env.get_template(f"{target}.jinja")
        return template

    def generate_file(self, target: str, **kwargs) -> pathlib.Path:
        template = self._load_template(target)
        output_path = self._output_dir / target
        template.stream(**kwargs).dump(str(output_path.resolve()))
        print(f"Generated file: {output_path.resolve()}")
        return output_path

    @abc.abstractmethod
    def generate(self, specs_paths: Iterable[str] | Iterable[pathlib.Path] = None) -> tuple[pathlib.Path,...]:
        raise NotImplementedError
