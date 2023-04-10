from typing import List, Any

from obcpy.utils import obc_time

from . import spec

class OBCCmd:
    """Represents a command that will be sent to the OBC.

    Attributes:
        cmd_sys_spec: The command system specification for the command.
        inst_id: The unique instance ID of the command.
        args: The arguments to the command.
        date_time: When the command should be executed (either immediately or at the specified time).
    """

    def __init__(self, cmd_sys_spec: spec.OBCCmdSysSpec, inst_id: int, args: List[Any], date_time: obc_time.OBCDateTime):
        self._cmd_sys_spec = cmd_sys_spec
        self._inst_id = inst_id
        self._args = args
        self._date_time = date_time

    @property
    def cmd_sys_spec(self) -> spec.OBCCmdSysSpec:
        return self._cmd_sys_spec

    @property
    def inst_id(self) -> int:
        return self._inst_id

    @property
    def args(self) -> List[Any]:
        return self._args

    @property
    def date_time(self) -> obc_time.OBCDateTime:
        return self._date_time
