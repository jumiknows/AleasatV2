from typing import Iterable, Tuple
from enum import IntEnum

from obcpy.data_fmt import data_field_impl
from obcpy.utils.data import bit

class RegisterSet:
    DATA_FIELD_ADDR = data_field_impl.DataFieldImpl.create_data_field("addr", "u16")
    DATA_FIELD_DATA = data_field_impl.DataFieldImpl.create_data_field("data", "u8")

    def __init__(self, regs: Iterable[Tuple[int, int]]):
        self.regs = regs

    def serialize(self) -> bytes:
        data = bytearray()
        for reg in self.regs:
            data.extend(RegisterSet.DATA_FIELD_ADDR.encode(reg[0])) # Address
            data.extend(RegisterSet.DATA_FIELD_DATA.encode(reg[1])) # Data
        return data

class RegAddr:
    def __init__(self, addr: int):
        self._addr = addr

    @property
    def ADDR(self) -> int:
        return self._addr

class TIMING:
    HS_1       = RegAddr(0x3800)
    HS_0       = RegAddr(0x3801)
    VS_1       = RegAddr(0x3802)
    VS_0       = RegAddr(0x3803)
    HW_1       = RegAddr(0x3804)
    HW_0       = RegAddr(0x3805)
    VH_1       = RegAddr(0x3806)
    VH_0       = RegAddr(0x3807)
    DVPHO_1    = RegAddr(0x3808)
    DVPHO_0    = RegAddr(0x3809)
    DVPVO_1    = RegAddr(0x380A)
    DVPVO_0    = RegAddr(0x380B)
    HTS_1      = RegAddr(0x380C)
    HTS_0      = RegAddr(0x380D)
    VTS_1      = RegAddr(0x380E)
    VTS_0      = RegAddr(0x380F)
    HVOFFS     = RegAddr(0x3810)
    THNVH      = RegAddr(0x3811)
    THNHW_0    = RegAddr(0x3812)
    THNHW_1    = RegAddr(0x3813)
    CONTROL_18 = RegAddr(0x3818)

class ISP:
    CONTROL_00 = RegAddr(0x5000)
    class CONTROL_01:
        ADDR = 0x5001
        class VALUE:
            SDE_EN     = bit(7)
            UV_ADJ_EN  = bit(6)
            VSCALE_EN  = bit(5)
            HSCALE_EN  = bit(4)
            LIN_STR_EN = bit(3)
            UV_AVG_EN  = bit(2)
            COL_MAT_EN = bit(1)
            AWB_EN     = bit(0)
    CONTROL_02 = RegAddr(0x5002)
    CONTROL_03 = RegAddr(0x5003)

class AWB:
    R_GAIN_1         = RegAddr(0x3400)
    R_GAIN_0         = RegAddr(0x3401)
    G_GAIN_1         = RegAddr(0x3402)
    G_GAIN_0         = RegAddr(0x3403)
    B_GAIN_1         = RegAddr(0x3404)
    B_GAIN_0         = RegAddr(0x3405)
    class MODE:
        ADDR = 0x3406
        class VALUE:
            AUTO   = 0
            MANUAL = bit(0)

    CONTROL_00       = RegAddr(0x5180)
    CONTROL_01       = RegAddr(0x5181)
    CONTROL_02       = RegAddr(0x5182)
    class CONTROL_03:
        ADDR = 0x5183
        class VALUE:
            ADVANCED = 0
            SIMPLE   = bit(7)
    CONTROL_04       = RegAddr(0x5184)
    CONTROL_05       = RegAddr(0x5185)
    CONTROL_06       = RegAddr(0x5186)
    CONTROL_07       = RegAddr(0x5187)
    CONTROL_08       = RegAddr(0x5188)
    CONTROL_09       = RegAddr(0x5189)
    CONTROL_10       = RegAddr(0x518A)
    CONTROL_11       = RegAddr(0x518B)
    CONTROL_12       = RegAddr(0x518C)
    CONTROL_13       = RegAddr(0x518D)
    CONTROL_14       = RegAddr(0x518E)
    CONTROL_15       = RegAddr(0x518F)
    CONTROL_16       = RegAddr(0x5190)
    CONTROL_17       = RegAddr(0x5191)
    CONTROL_18       = RegAddr(0x5192)
    CONTROL_19       = RegAddr(0x5193)
    CONTROL_20       = RegAddr(0x5194)
    CONTROL_21       = RegAddr(0x5195)
    CONTROL_22       = RegAddr(0x5196)
    CONTROL_23       = RegAddr(0x5197)
    R_GAIN_MANUAL_0  = RegAddr(0x5198)
    R_GAIN_MANUAL_1  = RegAddr(0x5199)
    G_GAIN_MANUAL_0  = RegAddr(0x519A)
    G_GAIN_MANUAL_1  = RegAddr(0x519B)
    B_GAIN_MANUAL_0  = RegAddr(0x519C)
    B_GAIN_MANUAL_1  = RegAddr(0x519D)
    CONTROL_30       = RegAddr(0x519E)
    CURRENT_R_GAIN_0 = RegAddr(0x519F)
    CURRENT_R_GAIN_1 = RegAddr(0x51A0)
    CURRENT_G_GAIN_0 = RegAddr(0x51A1)
    CURRENT_G_GAIN_1 = RegAddr(0x51A2)
    CURRENT_B_GAIN_0 = RegAddr(0x51A3)
    CURRENT_B_GAIN_1 = RegAddr(0x51A4)
    AVERAGE_R        = RegAddr(0x51A5)
    AVERAGE_G        = RegAddr(0x51A6)
    AVERAGE_B        = RegAddr(0x51A7)

class SDE:
    class CONTROL_00:
        ADDR = 0x5580
        class VALUE:
            FIX_Y_EN    = bit(7)
            NEG_Y_EN    = bit(6)
            GRAY_IMG_EN = bit(5)
            FIX_V_EN    = bit(4)
            FIX_U_EN    = bit(3)
            CONTRAST_EN = bit(2)
            SAT_EN      = bit(1)
            HUE_EN      = bit(0)
    CONTROL_01 = RegAddr(0x5581)
    CONTROL_02 = RegAddr(0x5582)
    CONTROL_03 = RegAddr(0x5583)
    CONTROL_04 = RegAddr(0x5584)
    CONTROL_05 = RegAddr(0x5585)
    CONTROL_06 = RegAddr(0x5586)
    CONTROL_07 = RegAddr(0x5587)
    CONTROL_08 = RegAddr(0x5588)
    CONTROL_09 = RegAddr(0x5589)
    class CONTROL_10:
        ADDR = 0x558A
        class VALUE:
            SOL_EN               = bit(7)
            HUE_ANGLE_EN         = bit(6)
            COS_SIGN_CR          = bit(5)
            COS_SIGN_CB          = bit(4)
            CONTRAST_OFFSET_SIGN = bit(3)
            CONTRAST_BRIGHT_SIGN = bit(2)
            SIN_SIGN_CB          = bit(1)
            SIN_SIGN_CR          = bit(0)
