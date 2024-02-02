import os
import shutil
import subprocess
import sys
import tempfile

import PyInstaller.__main__

from main import *

EXECUTABLE_NAME = "SanAntonio"

def get_relative_path(path, parent=True):
    if parent:
        return str(path.relative_to(REPO_ROOT_PATH).parent) 
    else: 
        return str(path.relative_to(REPO_ROOT_PATH)) 

def replace_parent_path_with_temp(file_path, parent_path):
    if not os.path.isfile(file_path):
        raise FileNotFoundError(f"The file '{file_path}' does not exist.")

    with open(file_path, 'r') as file:
        content = file.read()

    modified_content = content.replace(parent_path, '$TEMP')

    with open(file_path, 'w') as file:
        file.write(modified_content)

if __name__ == "__main__":
    # Generate requirements.txt file
    result = subprocess.run(
        ["pipenv", "requirements"], capture_output=True, text=True, check=True
    )

    with open("requirements.txt", "w") as requirements_file:
        requirements_file.write(result.stdout)

    # arguments to PyInstaller
    # Creates a single binary from main.py
    args = [
        "main.py",
        "--onefile",
        "--noconfirm",
        "--name=SanAntonio",
        "--clean",
        f"--paths={TOOLS_DIR_PATH}:{get_relative_path(OBCPY_PATH)}",
    ]

    # Change paths
    for path in CMD_SYS_SPECS_PATHS:
        args.append(
            f"--add-data={path}:./{get_relative_path(path)}"
        )

    args.append(
        f"--add-data={str(LOG_SPECS_PATH)}:./{str(LOG_SPECS_PATH.relative_to(REPO_ROOT_PATH).parent)}"
    )

    for path in TASK_SPECS_PATHS:
        args.append(
            f"--add-data={path}:./{str(path.relative_to(REPO_ROOT_PATH).parent)}"
        )

    args.append(
            f"--add-data=./sanantonio/graphics/:./graphics/"
    )

    args.append(
        f"--collect-all={str(TOOLS_DIR_PATH.relative_to(REPO_ROOT_PATH).parent)}"
    )

    args.append(f"--collect-all={str(OBCPY_PATH.relative_to(REPO_ROOT_PATH).parent)}")

    PyInstaller.__main__.run(args)
