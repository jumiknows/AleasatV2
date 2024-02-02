import sanantonio
import os
import pathlib
import sys

REPO_ROOT_PATH = pathlib.Path(__file__).resolve().parent.parent
TOOLS_DIR_PATH = REPO_ROOT_PATH / "obc-firmware" / "tools"
OBCPY_PATH = REPO_ROOT_PATH / "obcpy" / "obcpy"

CMD_SYS_SPECS_PATHS = [
    TOOLS_DIR_PATH / "cmd_sys" / "cmd_sys.json",
    TOOLS_DIR_PATH / "cmd_sys" / "cmd_sys_test.json",
]
LOG_SPECS_PATH = TOOLS_DIR_PATH / "logging" / "log_specs.json"
TASK_SPECS_PATHS = [
    TOOLS_DIR_PATH / "rtos" / "obc_tasks.json"
]

if __name__ == "__main__":
    if getattr(sys, 'frozen', False):
        # executed if run as an executable
        bundle_dir = getattr(sys, '_MEIPASS', os.path.abspath(os.path.dirname(__file__)))

        CMD_SYS_SPECS_PATHS = [pathlib.Path(bundle_dir) / path.relative_to(REPO_ROOT_PATH) for path in CMD_SYS_SPECS_PATHS]
        LOG_SPECS_PATH = pathlib.Path(bundle_dir) / LOG_SPECS_PATH.relative_to(REPO_ROOT_PATH)
        TASK_SPECS_PATHS = [pathlib.Path(bundle_dir) / path.relative_to(REPO_ROOT_PATH) for path in TASK_SPECS_PATHS]

    sa = sanantonio.SanAntonio(CMD_SYS_SPECS_PATHS, LOG_SPECS_PATH, TASK_SPECS_PATHS)
    sa.run()
