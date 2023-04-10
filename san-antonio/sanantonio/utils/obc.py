from typing import Union

from obcpy import cmd_sys
from obcpy.utils import exc

from . import console as console_utils

def check_cmd_sys_resp(resp: Union[cmd_sys.resp.OBCResponse, exc.OBCError], *expected_fields) -> bool:
    """Checks if a response to a command is valid and contains all of the expected fields.
    If any checks fail, an error message is printed using console_utils.print_err(...).

    Args:
        resp: The response to check, an exc.OBCError object or None.
        *expected_fields: The names of the fields that are expected to be in the response

    Returns:
        True if the response is valid and contains all of the expected fields, otherwise False.
    """

    # Handle errors
    if resp is None:
        console_utils.print_err("Did not receive a response")
        return False

    if isinstance(resp, exc.OBCError):
        console_utils.print_err(str(resp))
        return False

    if not isinstance(resp, cmd_sys.resp.OBCResponse):
        console_utils.print_err(f"Unexpected response object: {type(resp)}")

    if len(expected_fields) > 0:
        if not resp.data.has_fields:
            console_utils.print_err("Response has no fields")
            return False

        for field in expected_fields:
            if not (field in resp.data):
                console_utils.print_err(f"Response missing {field} field")
                return False

    return True
