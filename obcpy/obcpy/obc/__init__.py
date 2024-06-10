from . import obc_base
from .system import obc_system
from .camera import obc_camera
from .gps import obc_gps
from .eps import obc_eps

class OBC(obc_system.OBCSystemFeature, obc_camera.OBCCameraFeature, obc_gps.OBCGpsFeature, obc_eps.OBCEPSFeature, obc_base.OBCBase):
    pass
