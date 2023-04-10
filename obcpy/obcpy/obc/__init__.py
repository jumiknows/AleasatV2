from . import obc_base
from . import obc_system
from .camera import obc_camera

class OBC(obc_system.OBCSystemFeature, obc_camera.OBCCameraFeature, obc_base.OBCBase):
    pass
