import pathlib
import json
import argparse
from datetime import datetime

import jinja2

from obcpy.rtos import task_spec
from obcpy.utils import exc

BASE_NAME = "obc_tasks"

def main():
    # Parse and validate args
    parser = argparse.ArgumentParser(description="Generate OBC tasks header file.")
    parser.add_argument("-i", dest="specs_paths", action="store", required=True, nargs="+", help="Task specification (JSON) files")
    parser.add_argument("-o", dest="output_dir", action="store", required=True, help="Output folder")
    args = parser.parse_args()

    output_dir = pathlib.Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    # Load JSON specification files
    json_blobs = []

    for specs_path in args.specs_paths:
        specs_path = pathlib.Path(specs_path)
        print(f"Loading tasks from: {str(specs_path.absolute())}")
        with open(specs_path, "r") as specs_file:
            specs_json = json.load(specs_file)
            json_blobs.append(specs_json)

    # Build task specifications
    try:
        task_specs = task_spec.OBCTaskSpecs.from_json(*json_blobs)
        print(f"Loaded {task_specs.count} task specifications")
    except (exc.OBCTaskSpecError) as e:
        print(e)
        exit(1)

    # Prepare template environment
    template_dir = pathlib.Path(__file__).parent / "templates"
    file_loader = jinja2.FileSystemLoader(str(template_dir.resolve()))
    env = jinja2.Environment(
        loader=file_loader,
        keep_trailing_newline=True
    )

    # Generate IDs header
    template_ids_h = env.get_template(f"{BASE_NAME}_ids_gen_template.h")
    output_ids_h_path = output_dir / f"{BASE_NAME}_ids_gen.h"
    template_ids_h.stream(
        timestamp = datetime.now(),
        task_specs = task_specs,
    ).dump(str(output_ids_h_path.resolve()))
    print(f"Generated IDs header file: {str(output_ids_h_path.absolute())}")

    # Generate Header
    template_h = env.get_template(f"{BASE_NAME}_gen_template.h")
    output_h_path = output_dir / f"{BASE_NAME}_gen.h"
    template_h.stream(
        timestamp                = datetime.now(),
        obc_tasks_ids_gen_header = output_ids_h_path.name,
        task_specs               = task_specs,
    ).dump(str(output_h_path.resolve()))
    print(f"Generated header file: {str(output_h_path.absolute())}")

    # Generate Source
    template_c = env.get_template(f"{BASE_NAME}_gen_template.c")
    output_c_path = output_dir / f"{BASE_NAME}_gen.c"
    template_c.stream(
        timestamp            = datetime.now(),
        obc_tasks_gen_header = output_h_path.name,
        task_specs           = task_specs,
    ).dump(str(output_c_path.resolve()))
    print(f"Generated source file: {str(output_c_path.absolute())}")

if __name__ == "__main__":
    main()
