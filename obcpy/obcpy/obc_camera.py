from obcpy.cmd_sys import resp

from .obc_base import OBCBase
from .obc_base import DEFAULT_CMD_TIMEOUT

class OBCCameraFeature(OBCBase):
    """Implementation of camera features of the OBC
    """

    def camera_init(self, timeout=DEFAULT_CMD_TIMEOUT) -> resp.OBCResponse:
        return self.send_cmd("TEST_CAM_INIT", timeout=timeout)

    def camera_capture(self, timeout=DEFAULT_CMD_TIMEOUT) -> resp.OBCResponse:
        return self.send_cmd("TEST_CAM_CAPTURE", timeout=timeout)
