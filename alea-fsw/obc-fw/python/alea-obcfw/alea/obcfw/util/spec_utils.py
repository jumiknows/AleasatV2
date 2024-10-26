from types import ModuleType
from importlib.resources import files
import pathlib
import json

def load_json_files(paths: list[str | pathlib.Path]) -> list[dict]:
    all_data: list[dict] = []

    for path in paths:
        if not isinstance(path, pathlib.Path):
            path = pathlib.Path(path)

        print(f"Loading data from: {path.resolve()}")
        with open(path, "r") as json_file:
            json_data = json.load(json_file)
            all_data.append(json_data)

    return all_data

def load_json_resources(module: ModuleType, *filenames) -> list[dict]:
    all_data: list[dict] = []

    for filename in filenames:
        print(f"Loading data from: {filename}")
        file_text = files(module).joinpath(filename).read_text()
        json_data = json.loads(file_text)
        all_data.append(json_data)

    return all_data
