from . import ttc_base
from .spacecraft.obc import obc_feature
from .spacecraft.camera import camera_feature
from .spacecraft.gps import gps_feature
from .spacecraft.eps import eps_feature

class TTC(obc_feature.OBCFeature,
          camera_feature.CameraFeature,
          gps_feature.GPSFeature,
          eps_feature.EPSFeature,
          ttc_base.TTCBase):
    pass
