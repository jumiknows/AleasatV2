from obcpy import cmd_sys
import struct
from datetime import datetime

from . import gps_utils as UTILS

from obcpy.obc import obc_base
from obcpy.obc.obc_base import DEFAULT_CMD_TIMEOUT

class OBCGpsFeature(obc_base.OBCBase):
    """Implementation of GPS features of the OBC
    """

    def gps_get_pwr_mode(self, timeout: float = DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        return self.send_cmd("GPS_QUERY_POWER_MODE", timeout=timeout)

    def gps_set_pwr_mode(self, write_settings: UTILS.WriteSetting, power_save: bool, timeout: float = DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        return self.send_cmd("GPS_CONFIGURE_POWER_MODE", write_settings.value, power_save, timeout=timeout)

    def gps_get_software_crc(self, timeout: float = DEFAULT_CMD_TIMEOUT) ->  cmd_sys.resp.OBCResponse:
        return self.send_cmd("GPS_QUERY_SW_CRC", timeout=timeout)

    def gps_get_software_ver(self, timeout: float = DEFAULT_CMD_TIMEOUT) ->  cmd_sys.resp.OBCResponse:
        return self.send_cmd("GPS_QUERY_SW_VER", timeout=timeout)

    def gps_restart(self, restart_mode: UTILS.RestartMode, timeout: float = DEFAULT_CMD_TIMEOUT):
        now = datetime.now()
        date_bytes = struct.pack('HBBBBB', now.year, now.month, now.day, now.hour, now.minute, now.second)
        return self.send_cmd("GPS_RESTART", restart_mode.value, timeout=timeout)

    def gps_get_position_update_rate(self, timeout = DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        return self.send_cmd("GPS_QUERY_POSITION_UPDATE_RATE", timeout=timeout)

    def gps_set_position_update_rate(self, write_settings: UTILS.WriteSetting, position_update_rate: int, timeout = DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        return self.send_cmd("GPS_CONFIGURE_POSITION_UPATE_RATE", write_settings.value, position_update_rate, timeout=timeout)

    def gps_set_nav_msg_interval(self, write_settings: UTILS.WriteSetting, nav_msg_interval: int, timeout = DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        return self.send_cmd("GPS_CONFIGURE_NAV_MSG_INTERVAL", write_settings.value, nav_msg_interval, timeout=timeout)

    def gps_set_nmea_settings(self, write_settings: UTILS.WriteSetting, gga: int, gsa: int, gsv: int, gll: int, rmc: int, vtg: int, zda: int, timeout = DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        return self.send_cmd("GPS_CONFIGURE_NMEA_MESSGES", write_settings.value, gga, gsa, gsv, gll, rmc, vtg, zda, timeout=timeout)

    def gps_get_nmea_settings(self, timeout = DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        return self.send_cmd("GPS_QUERY_NMEA_MESSGES", timeout=timeout)