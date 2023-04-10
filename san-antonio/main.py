import pathlib

import sanantonio

REPO_ROOT_PATH = pathlib.Path(__file__).resolve().parent.parent
TOOLS_DIR_PATH = REPO_ROOT_PATH / "obc-firmware" / "tools"

CMD_SYS_SPECS_PATHS = [
    TOOLS_DIR_PATH / "cmd_sys" / "cmd_sys.json",
    TOOLS_DIR_PATH / "cmd_sys" / "cmd_sys_test.json",
]
LOG_SPECS_PATH = TOOLS_DIR_PATH / "logging" / "log_specs.json"

if __name__ == "__main__":
    sa = sanantonio.SanAntonio(CMD_SYS_SPECS_PATHS, LOG_SPECS_PATH)
    sa.run()
