from typing import Tuple, Any

from obcpy import cmd_sys
from obcpy.data_fmt import data_field
from obcpy.utils import data as data_utils

class OBCCmdRespBasePanel:
    NO_DATA = "-"
    DATA_TOO_LARGE = "Data too large"

    MAX_FIELD_SIZE = 256

    def format_args_str(self, cmd: cmd_sys.cmd.OBCCmd):
        """Format the arguments of a command into a multi-line string for display in a table.

        Args:
            cmd: The command containing arguments to format.

        Returns:
            A string representation of the command arguments.
        """

        args_str = self.NO_DATA
        if cmd.args is not None:
            def map_func(arg: Tuple[data_field.DataField, Any]):
                # TODO limit size of argument for display
                if isinstance(arg[1], bytes) or isinstance(arg[1], bytearray):
                    arg_value = data_utils.bytes_to_hexstr(arg[1], try_decode=False)
                else:
                    arg_value = str(arg[1])

                return f"{arg[0].name} = {arg_value}"

            args_str = "\n".join(map(map_func, zip(cmd.cmd_sys_spec.args, cmd.args)))
        return args_str

    def format_resp_data_str(self, resp: cmd_sys.resp.OBCResponse):
        """Format the data fields of a response into a multi-line string for display in a table.

        Args:
            resp: The response containing data to format.

        Returns:
            A string representation of the response data. If any field is larger than MAX_FIELD_SIZE,
            based on the return value of len(<field value>), then the DATA_TOO_LARGE string will be
            substituted for the actual field data.
        """

        data_str = self.NO_DATA
        if resp.data.error is not None:
            data_str = f"ERROR: {resp.data.error}"
        elif resp.data.has_fields:
            if len(resp.data.fields) > 0:
                # Function to generate string representation of each field
                def map_func(name: str) -> str:
                    field_value = ""
                    if self._check_size(resp.data.fields[name]):
                        field_value = str(resp.data.fields[name])
                    else:
                        field_value = self.DATA_TOO_LARGE
                    return f"{name} = {field_value}"

                data_str = "\n".join(map(map_func, resp.data.fields))
        else:
            if self._check_size(resp.data):
                data_str = data_utils.bytes_to_hexstr(resp.data, try_decode=False)
            else:
                data_str = self.DATA_TOO_LARGE
        return data_str

    def _check_size(self, obj) -> bool:
        """Returns False, if obj supports len() and has a length greater than MAX_FIELD_SIZE,
        otherwise returns True.
        """
        try:
            if len(obj) > self.MAX_FIELD_SIZE:
                return False
        except TypeError:
            pass
        return True
