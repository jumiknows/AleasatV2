from .obc_base import OBCBase
from .obc_system import OBCSystemFeature
from .obc_camera import OBCCameraFeature

class OBC(OBCSystemFeature, OBCCameraFeature, OBCBase):
    pass
