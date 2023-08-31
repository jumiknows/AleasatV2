from . import obc_base
from . import obc_system
from .camera import obc_camera
from .gps import obc_gps 

class OBC(obc_system.OBCSystemFeature, obc_camera.OBCCameraFeature, obc_gps.OBCGpsFeature, obc_base.OBCBase):
    pass
