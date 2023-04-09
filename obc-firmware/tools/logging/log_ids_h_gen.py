#!/usr/bin/local/python3
import argparse
import sys
import json
from jinja2 import Environment, FileSystemLoader
import pathlib
import datetime

"""
LOGGER IDS GENERATOR
    - Generates a log id header from a json file (log_ids.json -> log_ids.h)
"""

def main():
    """
    Generates a header file from log_ids.json
    """

    # Parse and validate args
    parser = argparse.ArgumentParser(description='Generate log ids header file.')
    parser.add_argument('-i', dest='inputFile', action='store', required=True,
                    help='Input template header file path')
    parser.add_argument('-o', dest='outputFile', action='store', required=True,
                    help='Output header file path')
    parser.add_argument('-t', dest='templateFile', action='store', required=True,
                    help='Template file for header')

    args = parser.parse_args()


    # Open and parse json file
    with open(args.inputFile, 'r') as log_ids_json:
        log_ids = json.load(log_ids_json)

    # Find the maximum log ID value
    max_id = 0
    for entry in log_ids:
        this_id = int(log_ids[entry]['id'])
        if this_id > max_id:
            max_id = this_id

    # Add MAX_LOG_ID_VALUE to end of list
    log_ids['MAX_LOG_ID_VALUE'] = {
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
    template.stream(log_ids=log_ids, date=datetime.datetime.now().strftime("%I:%M %p on %B %d, %Y")) \
        .dump(args.outputFile)

    print(f"Finished generating log ids file. Number of IDs: {len(log_ids)}")

if __name__ == '__main__':
    print("ALEASAT Logger IDs Generator")
    main()
