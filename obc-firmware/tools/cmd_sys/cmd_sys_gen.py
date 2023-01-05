import pathlib
import json
import argparse
from datetime import datetime

import jinja2

from obcpy.cmd_sys import spec
from obcpy.utils import exc

BASE_NAME = "cmd_sys_gen"

def main():
    # Parse and validate args
    parser = argparse.ArgumentParser(description="Generate log ids header file.")
    parser.add_argument("-i", dest="specs_paths", action="store", required=True, nargs="+", help="Command system specification (JSON) files")
    parser.add_argument("-o", dest="output_dir", action="store", required=True, help="Output folder")
    args = parser.parse_args()

    output_dir = pathlib.Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    # Load JSON specification files
    json_blobs = []

    for specs_path in args.specs_paths:
        specs_path = pathlib.Path(specs_path)
        print(f"Loading commands from: {str(specs_path.absolute())}")
        with open(specs_path, "r") as specs_file:
            specs_json = json.load(specs_file)
            json_blobs.append(specs_json)

    # Build command system specifications
    try:
        cmd_specs = spec.OBCCmdSysSpecs.from_json(*json_blobs)
        print(f"Loaded {cmd_specs.count} command specifications")
    except (exc.OBCCmdSysSpecError, exc.OBCDataFieldError) as e:
        print(e)
        exit(1)

    # Prepare template environment
    template_dir = pathlib.Path(__file__).parent / "templates"
    file_loader = jinja2.FileSystemLoader(str(template_dir.resolve()))
    env = jinja2.Environment(
        loader=file_loader,
        keep_trailing_newline=True
    )

    # Generate Header
    template_h = env.get_template(f"{BASE_NAME}_template.h")
    output_h_path = output_dir / f"{BASE_NAME}.h"
    template_h.stream(
        timestamp = datetime.now(),
        cmd_specs = cmd_specs,
    ).dump(str(output_h_path.resolve()))
    print(f"Generated header file: {str(output_h_path.absolute())}")

    # Generate Source
    template_c = env.get_template(f"{BASE_NAME}_template.c")
    output_c_path = output_dir / f"{BASE_NAME}.c"
    template_c.stream(
        timestamp          = datetime.now(),
        cmd_sys_gen_header = output_h_path.name,
        cmd_specs          = cmd_specs,
    ).dump(str(output_c_path.resolve()))
    print(f"Generated source file: {str(output_c_path.absolute())}")

if __name__ == "__main__":
    main()
