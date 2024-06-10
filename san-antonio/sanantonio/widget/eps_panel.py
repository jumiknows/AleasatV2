from typing import Union

from PyQt5 import QtCore, QtGui, QtWidgets

from obcpy import cmd_sys
from obcpy.utils import exc
from obcpy.obc.eps import obc_eps

from sanantonio.backend import obcqt
from sanantonio.ui.control_panels import eps_panel_ui
from sanantonio.utils import obc as obc_utils
from sanantonio.utils import console as console_utils
from sanantonio.utils import graphics

class EPSControl(QtWidgets.QWidget):
    GREEN_CIRCLE = None
    RED_CIRCLE   = None

    refresh_requested = QtCore.pyqtSignal()

    def __init__(self, obc_provider: obcqt.OBCInterfaceProvider, name: str, write_cmd: obc_eps.EPSWriteCmd, invert: bool, read_cmd: obc_eps.EPSReadIntCmd, read_bit: int, allow_on: bool = True, allow_off: bool = True, parent=None):
        super().__init__(parent)

        # Lazy initialize pixmaps
        if EPSControl.GREEN_CIRCLE is None:
            EPSControl.GREEN_CIRCLE = QtGui.QPixmap(str(graphics.GRAPHICS_DIR / "green-circle-small.png"))
        if EPSControl.RED_CIRCLE is None:
            EPSControl.RED_CIRCLE = QtGui.QPixmap(str(graphics.GRAPHICS_DIR / "red-circle-small.png"))

        self.obc_provider = obc_provider

        self._write_cmd = write_cmd
        self._invert    = invert
        self._read_cmd  = read_cmd
        self._read_bit  = read_bit

        self.container = QtWidgets.QHBoxLayout(self)

        self.icon = QtWidgets.QLabel(self)
        self.icon.setText("")
        self.icon.setPixmap(EPSControl.RED_CIRCLE)
        self.container.addWidget(self.icon)

        self.label = QtWidgets.QLabel(self)
        self.label.setText(name)
        self.container.addWidget(self.label)

        self.btn_on = QtWidgets.QPushButton(self)
        self.btn_on.setText("ON")
        self.btn_on.clicked.connect(self.handle_on)
        self.btn_on.setEnabled(allow_on)
        self.container.addWidget(self.btn_on)

        self.btn_off = QtWidgets.QPushButton(self)
        self.btn_off.setText("OFF")
        self.btn_off.clicked.connect(self.handle_off)
        self.btn_off.setEnabled(allow_off)
        self.container.addWidget(self.btn_off)

        self.btn_force_on = QtWidgets.QPushButton(self)
        self.btn_force_on.setText("FORCE ON")
        self.btn_force_on.clicked.connect(self.handle_force_on)
        self.btn_force_on.setEnabled(allow_on)
        self.container.addWidget(self.btn_force_on)

        self.btn_force_off = QtWidgets.QPushButton(self)
        self.btn_force_off.setText("FORCE OFF")
        self.btn_force_off.clicked.connect(self.handle_force_off)
        self.btn_force_off.setEnabled(allow_off)
        self.container.addWidget(self.btn_force_off)

    @property
    def obc(self) -> obcqt.OBCQT:
        return self.obc_provider.obc

    def set_state(self, on: bool):
        self.icon.setPixmap(EPSControl.GREEN_CIRCLE if on else EPSControl.RED_CIRCLE)

    @QtCore.pyqtSlot()
    def handle_on(self):
        state = obc_eps.EPSState.OFF if self._invert else obc_eps.EPSState.ON
        self._eps_write(self._write_cmd, state)

    @QtCore.pyqtSlot()
    def handle_off(self):
        state = obc_eps.EPSState.ON if self._invert else obc_eps.EPSState.OFF
        self._eps_write(self._write_cmd, state)

    @QtCore.pyqtSlot()
    def handle_force_on(self):
        state = obc_eps.EPSState.FORCED_OFF if self._invert else obc_eps.EPSState.FORCED_ON
        self._eps_write(self._write_cmd, state)

    @QtCore.pyqtSlot()
    def handle_force_off(self):
        state = obc_eps.EPSState.FORCED_ON if self._invert else obc_eps.EPSState.FORCED_OFF
        self._eps_write(self._write_cmd, state)

    def _eps_write(self, id: obc_eps.EPSWriteCmd, state: obc_eps.EPSState):
        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.eps_write(id, state),
            self._eps_write_callback
        ))

    def _eps_write_callback(self, resp: Union[cmd_sys.resp.OBCResponse, exc.OBCError]):
        self.refresh_requested.emit()

    @QtCore.pyqtSlot(cmd_sys.resp.OBCResponse)
    def handle_refresh_completed(self, resp: cmd_sys.resp.OBCResponse):
        on = False

        if self._read_cmd == obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1:
            on = ((resp.data["out_cond_1"] >> self._read_bit) & 0b1) == 1
        elif self._read_cmd == obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_2:
            on = ((resp.data["out_cond_2"] >> self._read_bit) & 0b1) == 1

        if self._invert:
            on = not on

        self.set_state(on)

class EpsPanel(QtWidgets.QWidget, eps_panel_ui.Ui_EpsPanel):
    refresh_controls_completed = QtCore.pyqtSignal(cmd_sys.resp.OBCResponse)

    def __init__(self, obc_provider: obcqt.OBCInterfaceProvider, parent=None):
        super().__init__(parent)

        self.obc_provider = obc_provider

        # Declare UI members with type hints - these are assigned in setupUI()
        self.controls_box: QtWidgets.QGroupBox
        self.refresh_controls_btn: QtWidgets.QPushButton
        self.refresh_counters_btn: QtWidgets.QPushButton

        self.uptime_val: QtWidgets.QLineEdit
        self.pwr_on_cyc_val: QtWidgets.QLineEdit
        self.under_volt_val: QtWidgets.QLineEdit
        self.short_circ_val: QtWidgets.QLineEdit
        self.over_temp_val: QtWidgets.QLineEdit

        self.setupUi(self)

        self.counters = [
            self.uptime_val,
            self.pwr_on_cyc_val,
            self.under_volt_val,
            self.short_circ_val,
            self.over_temp_val
        ]

        self.controls = [
            self._create_eps_control("SW Self Lock",  obc_eps.EPSWriteCmd.SW_SELF_LOCK, False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 0),

            self._create_eps_control("Battery BUS",   obc_eps.EPSWriteCmd.V_BATT_EN,    False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 1),
            self._create_eps_control("BCR BUS",       obc_eps.EPSWriteCmd.BCR_OUT_EN,   False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 2),
            self._create_eps_control("3V3 BUS",       obc_eps.EPSWriteCmd.SHD_3V3,      False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 3, allow_off=False),
            self._create_eps_control("5V BUS",        obc_eps.EPSWriteCmd.SHD_5V,       False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 4),
            self._create_eps_control("3V3 LUP BUS",   obc_eps.EPSWriteCmd.LUP_3V3,      True,  obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_2, 0, allow_off=False),
            self._create_eps_control("5V LUP BUS",    obc_eps.EPSWriteCmd.LUP_5V,       True,  obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_2, 1),

            self._create_eps_control("Charge Enable", obc_eps.EPSWriteCmd.SHD_CHARGE,   True,  obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_2, 2),
            self._create_eps_control("Fast Charge 1", obc_eps.EPSWriteCmd.CHARGE_I1,    False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_2, 3),
            self._create_eps_control("Fast Charge 2", obc_eps.EPSWriteCmd.CHARGE_I2,    False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_2, 4),

            self._create_eps_control("OUT 1",         obc_eps.EPSWriteCmd.OUT1,         False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 7),
            self._create_eps_control("OUT 2",         obc_eps.EPSWriteCmd.OUT2,         False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 8),
            self._create_eps_control("OUT 3",         obc_eps.EPSWriteCmd.OUT3,         False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 9),
            self._create_eps_control("OUT 4",         obc_eps.EPSWriteCmd.OUT4,         False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 10),
            self._create_eps_control("OUT 5",         obc_eps.EPSWriteCmd.OUT5,         False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 11),
            self._create_eps_control("OUT 6",         obc_eps.EPSWriteCmd.OUT6,         False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 12),

            self._create_eps_control("Heater 1",      obc_eps.EPSWriteCmd.HEATER_1,     False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 13),
            self._create_eps_control("Heater 2",      obc_eps.EPSWriteCmd.HEATER_2,     False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 14),
            self._create_eps_control("Heater 3",      obc_eps.EPSWriteCmd.HEATER_3,     False, obc_eps.EPSReadIntCmd.EPS_READ_OUTPUT_CONDITIONS_1, 15),
        ]

        for i, control in enumerate(self.controls):
            self.controls_box.layout().addWidget(control, (i % 10), (i // 10))

        # Connect signals / slots
        self.refresh_controls_btn.clicked.connect(self._handle_refresh_controls)
        self.refresh_counters_btn.clicked.connect(self._handle_refresh_counters)

    @property
    def obc(self) -> obcqt.OBCQT:
        return self.obc_provider.obc

    def _create_eps_control(self, name: str, write_cmd: obc_eps.EPSWriteCmd, invert: bool, read_cmd: obc_eps.EPSReadIntCmd, read_bit: int, allow_on: bool = True, allow_off: bool = True) -> EPSControl:
        control = EPSControl(self.obc_provider, name, write_cmd, invert, read_cmd, read_bit, allow_on=allow_on, allow_off=allow_off, parent=self)
        control.refresh_requested.connect(self._handle_refresh_controls)
        self.refresh_controls_completed.connect(control.handle_refresh_completed)
        return control

    @QtCore.pyqtSlot()
    def _handle_refresh_controls(self):
        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.eps_read_status(),
            self._refresh_controls_callback
        ))

    def _refresh_controls_callback(self, resp: Union[cmd_sys.resp.OBCResponse, exc.OBCError]):
        if not obc_utils.check_cmd_sys_resp(resp, "eps_err", "out_cond_1", "out_cond_2"):
            return

        if resp.data["eps_err"] != 0:
            console_utils.print_err(f"EPS error during controls refresh: {resp.data['eps_err']}")
            return

        self.refresh_controls_completed.emit(resp)

    @QtCore.pyqtSlot()
    def _handle_refresh_counters(self):
        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.eps_read_counters(),
            self._refresh_counters_callback
        ))

    def _refresh_counters_callback(self, resp: Union[cmd_sys.resp.OBCResponse, exc.OBCError]):
        if not obc_utils.check_cmd_sys_resp(resp, "eps_err", "uptime", "power_on_cycles", "under_voltage", "short_circuit", "over_temp"):
            return

        if resp.data["eps_err"] != 0:
            console_utils.print_err(f"EPS error during counters refresh: {resp.data['eps_err']}")
            return

        self.uptime_val.setText(str(resp.data["uptime"]))
        self.pwr_on_cyc_val.setText(str(resp.data["power_on_cycles"]))
        self.under_volt_val.setText(str(resp.data["under_voltage"]))
        self.short_circ_val.setText(str(resp.data["short_circuit"]))
        self.over_temp_val.setText(str(resp.data["over_temp"]))

    def reset_eps_panel(self):
        for control in self.controls:
            control.set_state(False)

        for counter in self.counters:
            counter.setText("")
