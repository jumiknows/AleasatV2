from enum import IntEnum

from obcpy import cmd_sys

from obcpy.obc import obc_base
from obcpy.obc.obc_base import DEFAULT_CMD_TIMEOUT

class EPSWriteCmd(IntEnum):
    SW_SELF_LOCK        = 0x00 # Self lock switch
    V_BATT_EN           = 0x01 # Battery bus
    BCR_OUT_EN          = 0x02 # BCR bus
    SHD_3V3             = 0x03 # 3V3 bus
    SHD_5V              = 0x04 # 5V bus
    LUP_3V3             = 0x05 # Latch-up protected 3V3 bus (1 = OFF)
    LUP_5V              = 0x06 # Latch-up protected 5V bus (1 = OFF)
    SHD_CHARGE          = 0x07 # Battery charger suppresion/shutdown (1 = Charging OFF)
    CHARGE_I1           = 0x08 # Fast/slow battery charge 1
    CHARGE_I2           = 0x09 # Fast/slow battery charge 2
    OUT1                = 0x0A # Output 1 / payload
    OUT2                = 0x0B # Output 2
    OUT3                = 0x0C # Output 3
    OUT4                = 0x0D # Output 4 / OBC
    OUT5                = 0x0E # Output 5 / UHF
    OUT6                = 0x0F # Output 6
    HEATER_1            = 0x10 # Battery heater 1
    HEATER_2            = 0x11 # Battery heater 2
    HEATER_3            = 0x12 # Battery heater 3
    I2C_PULLUP_10K      = 0x13 # 10k I2C pullup resistor
    I2C_PULLUP_4K7      = 0x14 # 4.7k I2C pullup resistor
    RS485_TERM_RESISTOR = 0x15 # Enable the 120 ohm Termination Resistor at RS485

class EPSReadIntCmd(IntEnum):
    EPS_READ_INPUT_CONDITION     = 23 # Input condition bitfield
    EPS_READ_OUTPUT_CONDITIONS_1 = 24 # Output conditions 1 bitfield
    EPS_READ_OUTPUT_CONDITIONS_2 = 25 # Output conditions 2 bitfield
    EPS_READ_POWER_ON_CYCLES     = 26 # Power on cycles counter
    EPS_READ_V_UNDER_VOLTAGE     = 27 # V under voltage event counter
    EPS_READ_V_SHORT_CIRCUIT     = 28 # V short circuit event counter
    EPS_READ_V_OVER_TEMPERATURE  = 29 # V over temperature event counter
    # ...
    EPS_READ_SOFTWARE_VERSION    = 42 # Software version
    EPS_READ_DEFAULTS_1          = 43 # Read defaults 1 bitfield
    EPS_READ_DEFAULTS_2          = 44 # Read defaults 2 bitfield
    # ...
    EPS_READ_CHARGE_CYCLES       = 48 # Charge cycles counter
    # ...
    EPS_READ_UPTIME_HIGHER_WORD  = 51 # Upper 16 bits of uptime counter (measured in seconds)
    EPS_READ_UPTIME_LOWER_WORD   = 52 # lower 16 bits of uptime counter (measured in seconds)

class EPSState(IntEnum):
    OFF        = 0
    ON         = 1
    FORCED_OFF = 2
    FORCED_ON  = 3

class OBCEPSFeature(obc_base.OBCBase):
    """Implementation of eps features of the OBC
    """

    def eps_write(self, id: EPSWriteCmd, state: EPSState, timeout: float = DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        return self.send_cmd("TEST_EPS_WRITE", id, state, timeout=timeout)

    def eps_read(self, id: int, timeout: float = DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        return self.send_cmd("TEST_EPS_READ_INT", id, timeout=timeout)

    def eps_measure_buses(self, timeout: float = DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        return self.send_cmd("TEST_EPS_MEASURE_BUSES", timeout=timeout)

    def eps_measure_temps(self, timeout: float = DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        return self.send_cmd("TEST_EPS_MEASURE_TEMPS", timeout=timeout)

    def eps_read_counters(self, timeout: float = DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        return self.send_cmd("TEST_EPS_READ_COUNTERS", timeout=timeout)

    def eps_read_status(self, timeout: float = DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        return self.send_cmd("TEST_EPS_READ_STATUS", timeout=timeout)
