#!/usr/bin/local/python3
import argparse
import json
import pathlib
import datetime

from jinja2 import Environment, FileSystemLoader

"""
LOG HEADER GENERATOR
    - Generates a header file from a log specifications JSON file
"""

def main():
    """
    Generates a header file from log specifications JSON file
    """

    # Parse and validate args
    parser = argparse.ArgumentParser(description='Generate log header file.')
    parser.add_argument('-i', dest='inputFile', action='store', required=True, help='Input log specifications JSON file path')
    parser.add_argument('-o', dest='outputFile', action='store', required=True, help='Output header file path')
    parser.add_argument('-t', dest='templateFile', action='store', required=True, help='Template file for header')

    args = parser.parse_args()


    # Open and parse json file
    with open(args.inputFile, 'r') as log_specs_json:
        log_specs = json.load(log_specs_json)

    # Find the maximum log ID value
    max_id = 0
    for entry in log_specs:
        this_id = int(log_specs[entry]['id'])
        if this_id > max_id:
            max_id = this_id

    # Add MAX_LOG_ID_VALUE to end of list
    log_specs['MAX_LOG_ID_VALUE'] = {
        'id': int(max_id),
        'description': 'Maximum log ID value. Should always be equal to the largest log ID.'
    }

    # Create file dirs
    pathlib.Path(args.outputFile).parent.mkdir(parents=True, exist_ok=True)

    # Split template file absolute path into parent directory + filename
    templateParent = pathlib.Path(args.templateFile).parent
    templateName = pathlib.Path(args.templateFile).name

    # Generate the header file
    file_loader = FileSystemLoader(templateParent)
    env = Environment(
        loader=file_loader,
        keep_trailing_newline=True
    )
    template = env.get_template(templateName)
    template.stream(
        log_specs = log_specs,
        timestamp = datetime.datetime.now()
    ).dump(args.outputFile)

    print(f"Finished generating log header file. Number of log specs: {len(log_specs)}")

if __name__ == '__main__':
    print("ALEASAT Log Header Generator")
    main()
