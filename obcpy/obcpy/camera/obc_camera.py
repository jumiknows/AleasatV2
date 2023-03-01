from collections import namedtuple

from obcpy.cmd_sys import resp

from obcpy.obc_base import OBCBase
from obcpy.obc_base import DEFAULT_CMD_TIMEOUT

from obcpy.utils.data import get_byte

from . import OV5642

class OBCCameraFeature(OBCBase):
    """Implementation of camera features of the OBC
    """

    def camera_init(self, timeout: float = DEFAULT_CMD_TIMEOUT) -> resp.OBCResponse:
        return self.send_cmd("TEST_CAM_INIT", timeout=timeout)

    def camera_capture(self, timeout: float = DEFAULT_CMD_TIMEOUT) -> resp.OBCResponse:
        return self.send_cmd("TEST_CAM_CAPTURE", timeout=timeout)

    def camera_write_sensor_reg(self, addr: int, data: int, timeout: float = DEFAULT_CMD_TIMEOUT) -> resp.OBCResponse:
        return self.send_cmd("TEST_CAM_WR_SREG", addr, data, timeout=timeout)

    def camera_read_sensor_reg(self, addr: int, timeout: float = DEFAULT_CMD_TIMEOUT) -> resp.OBCResponse:
        return self.send_cmd("TEST_CAM_RD_SREG", addr, timeout=timeout)

    def camera_write_sensor_regs(self, regs: OV5642.RegisterSet, timeout: float = DEFAULT_CMD_TIMEOUT) -> resp.OBCResponse:
        return self.send_cmd("TEST_CAM_WR_SREGS", regs.serialize(), timeout=timeout)

    def camera_set_resolution(self,
                          win_x: int, win_y: int, win_width: int, win_height: int,
                          out_width: int, out_height: int,
                          tot_width: int, tot_height: int,
                          timeout: float = DEFAULT_CMD_TIMEOUT) -> resp.OBCResponse:
        # Read ISP_CONTROL_01
        resp = self.camera_read_sensor_reg(OV5642.ISP.CONTROL_01.ADDR, timeout=timeout)
        if not resp.is_success:
            return resp

        # Set VSCALE_EN and HSCALE_EN bits
        isp_control_01_data = resp["data"]
        isp_control_01_data |= OV5642.ISP.CONTROL_01.VALUE.VSCALE_EN
        isp_control_01_data |= OV5642.ISP.CONTROL_01.VALUE.HSCALE_EN

        # Write registers
        regs = OV5642.RegisterSet([
            # Window start
            (OV5642.TIMING.HS_1.ADDR, get_byte(win_x, 1)),
            (OV5642.TIMING.HS_0.ADDR, get_byte(win_x, 0)),

            (OV5642.TIMING.VS_1.ADDR, get_byte(win_y, 1)),
            (OV5642.TIMING.VS_0.ADDR, get_byte(win_y, 0)),

            # Window size
            (OV5642.TIMING.HW_1.ADDR, get_byte(win_width, 1)),
            (OV5642.TIMING.HW_0.ADDR, get_byte(win_width, 0)),

            (OV5642.TIMING.VH_1.ADDR, get_byte(win_height, 1)),
            (OV5642.TIMING.VH_0.ADDR, get_byte(win_height, 0)),

            # Output size
            (OV5642.TIMING.DVPHO_1.ADDR, get_byte(out_width, 1)),
            (OV5642.TIMING.DVPHO_0.ADDR, get_byte(out_width, 0)),

            (OV5642.TIMING.DVPVO_1.ADDR, get_byte(out_height, 1)),
            (OV5642.TIMING.DVPVO_0.ADDR, get_byte(out_height, 0)),

            # Total size
            (OV5642.TIMING.HTS_1.ADDR, get_byte(tot_width, 1)),
            (OV5642.TIMING.HTS_0.ADDR, get_byte(tot_width, 0)),

            (OV5642.TIMING.VTS_1.ADDR, get_byte(tot_height, 1)),
            (OV5642.TIMING.VTS_0.ADDR, get_byte(tot_height, 0)),

            # ISP CONTROL 01
            (OV5642.ISP.CONTROL_01.ADDR, isp_control_01_data)
        ])

        resp = self.camera_write_sensor_regs(regs, timeout=timeout)
        return resp

    def camera_set_white_balance_manual(self, gain_red: int, gain_green: int, gain_blue: int, timeout: float = DEFAULT_CMD_TIMEOUT):
        regs = OV5642.RegisterSet([
            (OV5642.AWB.R_GAIN_1.ADDR, get_byte(gain_red, 1)),
            (OV5642.AWB.R_GAIN_0.ADDR, get_byte(gain_red, 0)),

            (OV5642.AWB.G_GAIN_1.ADDR, get_byte(gain_green, 1)),
            (OV5642.AWB.G_GAIN_0.ADDR, get_byte(gain_green, 0)),

            (OV5642.AWB.B_GAIN_1.ADDR, get_byte(gain_blue, 1)),
            (OV5642.AWB.B_GAIN_0.ADDR, get_byte(gain_blue, 0)),

            (OV5642.AWB.MODE.ADDR, OV5642.AWB.MODE.VALUE.MANUAL)
        ])

        return self.camera_write_sensor_regs(regs, timeout=timeout)

    def camera_set_white_balance_auto_simple(self, top_limit: int, bottom_limit: int, timeout: float = DEFAULT_CMD_TIMEOUT):
        regs = OV5642.RegisterSet([
            (OV5642.AWB.CONTROL_03.ADDR, OV5642.AWB.CONTROL_03.VALUE.SIMPLE),

            (OV5642.AWB.CONTROL_17.ADDR, top_limit),
            (OV5642.AWB.CONTROL_18.ADDR, bottom_limit),

            (OV5642.AWB.MODE.ADDR, OV5642.AWB.MODE.VALUE.AUTO)
        ])

        return self.camera_write_sensor_regs(regs, timeout=timeout)

    CameraContrast = namedtuple("CameraContrast", ["y_offset", "y_gain", "y_bright"])
    CameraSaturation = namedtuple("CameraSaturation", ["u", "v"])
    CameraHue = namedtuple("CameraHue", ["cos", "sin"])

    def camera_set_special_digital_effects(self, enable: bool = False,
                                           fixed_y: int = None,
                                           neg_y: bool = False,
                                           gray: bool = False,
                                           fixed_v: int = None,
                                           fixed_u: int = None,
                                           contrast: CameraContrast = None,
                                           saturation: CameraSaturation = None,
                                           hue: CameraHue = None,
                                           solarize: bool = False,
                                           timeout: float = DEFAULT_CMD_TIMEOUT):
        # Read ISP_CONTROL_01
        resp = self.camera_read_sensor_reg(OV5642.ISP.CONTROL_01.ADDR, timeout=timeout)
        if not resp.is_success:
            return resp

        # Assemble register values
        isp_control_01_data = resp["data"]
        if enable:
            # Set SDE bit
            isp_control_01_data |= OV5642.ISP.CONTROL_01.VALUE.SDE_EN
        else:
            # Clear SDE bit
            isp_control_01_data &= (~OV5642.ISP.CONTROL_01.VALUE.SDE_EN)

        reg_list = [
            (OV5642.ISP.CONTROL_01.ADDR, isp_control_01_data)
        ]

        sde_control_00_data = 0x0
        sde_control_10_data = 0x0

        if fixed_y is not None:
            sde_control_00_data |= OV5642.SDE.CONTROL_00.VALUE.FIX_Y_EN
            reg_list.append((OV5642.SDE.CONTROL_07.ADDR, fixed_y))

        if neg_y:
            sde_control_00_data |= OV5642.SDE.CONTROL_00.VALUE.NEG_Y_EN

        if gray:
            sde_control_00_data |= OV5642.SDE.CONTROL_00.VALUE.GRAY_IMG_EN

        if fixed_v is not None:
            sde_control_00_data |= OV5642.SDE.CONTROL_00.VALUE.FIX_V_EN
            reg_list.append((OV5642.SDE.CONTROL_06.ADDR, fixed_v))

        if fixed_u is not None:
            sde_control_00_data |= OV5642.SDE.CONTROL_00.VALUE.FIX_U_EN
            reg_list.append((OV5642.SDE.CONTROL_05.ADDR, fixed_u))

        if contrast is not None:
            sde_control_00_data |= OV5642.SDE.CONTROL_00.VALUE.CONTRAST_EN
            reg_list.extend([
                (OV5642.SDE.CONTROL_07.ADDR, contrast.y_offset),
                (OV5642.SDE.CONTROL_08.ADDR, contrast.y_gain),
                (OV5642.SDE.CONTROL_09.ADDR, contrast.y_bright),
            ])

        if saturation is not None:
            sde_control_00_data |= OV5642.SDE.CONTROL_00.VALUE.SAT_EN
            reg_list.extend([
                (OV5642.SDE.CONTROL_03.ADDR, saturation.u),
                (OV5642.SDE.CONTROL_04.ADDR, saturation.v),
            ])

        if hue is not None:
            sde_control_00_data |= OV5642.SDE.CONTROL_00.VALUE.HUE_EN
            reg_list.extend([
                (OV5642.SDE.CONTROL_01.ADDR, hue.cos),
                (OV5642.SDE.CONTROL_02.ADDR, hue.sin),
            ])

        if solarize:
            sde_control_10_data |= OV5642.SDE.CONTROL_10.VALUE.SOL_EN

        reg_list.extend([
            (OV5642.SDE.CONTROL_10.ADDR, sde_control_10_data),
            (OV5642.SDE.CONTROL_00.ADDR, sde_control_00_data)
        ])

        regs = OV5642.RegisterSet(reg_list)
        return self.camera_write_sensor_regs(regs, timeout=timeout)
