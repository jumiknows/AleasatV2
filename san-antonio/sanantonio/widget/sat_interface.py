from typing import Dict, Tuple

from PyQt5 import QtWidgets, QtCore

from obcpy.utils import serial as serial_utils
from obcpy.obc import obc_base
from obcpy.obc.protocol.app import app_log

from sanantonio.backend import obcqt
from sanantonio.ui import sat_interface_ui
from sanantonio.utils import ui as ui_utils

class SatInterface(QtWidgets.QWidget, sat_interface_ui.Ui_SatInterface, obcqt.OBCInterfaceProvider):
    _conn_state_changed = QtCore.pyqtSignal(bool)
    _obc_cmd_interface_changed = QtCore.pyqtSignal(object)

    _logs = QtCore.pyqtSignal(app_log.OBCLog)

    def __init__(self, obcs: Dict[int, obcqt.OBCQT], parent=None):
        super().__init__(parent)

        self._obcs = obcs

        self._cmd_interface = obc_base.OBCBase.InterfaceType.OBC_SERIAL
        self._serial_ports = []

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.obc_serial_if_btn: QtWidgets.QPushButton
        self.obc_serial_ports_cb: QtWidgets.QComboBox

        self.comms_serial_if_btn: QtWidgets.QPushButton
        self.comms_serial_ports_cb: QtWidgets.QComboBox

        self.refresh_devices_btn: QtWidgets.QPushButton

        self.connect_obc_serial_btn: QtWidgets.QPushButton
        self.connect_comms_serial_btn: QtWidgets.QPushButton

        self.setupUi(self)

        self._if_button_map = {
            obc_base.OBCBase.InterfaceType.OBC_SERIAL.id   : self.obc_serial_if_btn,
            obc_base.OBCBase.InterfaceType.COMMS_SERIAL.id : self.comms_serial_if_btn,
        }

        # Connect signals / slots
        self.obc_serial_if_btn.clicked.connect(self.use_obc_serial_interface)
        self.comms_serial_if_btn.clicked.connect(self.use_comms_serial_interface)

        self.refresh_devices_btn.clicked.connect(self.handle_refresh_clicked)

        self.connect_obc_serial_btn.clicked.connect(self.toggle_obc_serial_connection)
        self.connect_comms_serial_btn.clicked.connect(self.toggle_comms_serial_connection)

        # Update UI
        self._refresh_devices()
        self._update_obc_serial_connect_btn()
        self._update_comms_serial_connect_btn()
        self._update_cmd_interface()

    @property
    def obc(self) -> obcqt.OBCQT:
        # OBC for commands
        return self._obcs[self._cmd_interface.id]

    @property
    def logs(self) -> QtCore.pyqtBoundSignal:
        # Logs are only available on the OBC_SERIAL interface
        return self._obcs[obc_base.OBCBase.InterfaceType.OBC_SERIAL.id].logs

    @property
    def obc_serial_obc(self) -> obcqt.OBCQT:
        return self._obcs[obc_base.OBCBase.InterfaceType.OBC_SERIAL.id]

    @property
    def comms_serial_obc(self) -> obcqt.OBCQT:
        return self._obcs[obc_base.OBCBase.InterfaceType.COMMS_SERIAL.id]

    @property
    def conn_state_changed(self) -> QtCore.pyqtBoundSignal:
        return self._conn_state_changed

    @property
    def obc_cmd_interface_changed(self) -> QtCore.pyqtBoundSignal:
        return self._obc_cmd_interface_changed

    def get_obc_interface(self, interface_type: obc_base.OBCBase.InterfaceType) -> obcqt.OBCQT:
        return self._obcs[interface_type.id]

    @QtCore.pyqtSlot()
    def handle_refresh_clicked(self):
        self._refresh_devices()

    def _refresh_devices(self) -> None:
        # Remove all current ports from combo box
        for i in range(self.obc_serial_ports_cb.count()):
            self.obc_serial_ports_cb.removeItem(i)
        for i in range(self.comms_serial_ports_cb.count()):
            self.comms_serial_ports_cb.removeItem(i)

        # Get new ports
        new_ports = serial_utils.get_serial_ports()
        self._ports = list(dict.fromkeys(new_ports))

        # Update combo boxes
        self.obc_serial_ports_cb.addItems(self._ports)
        self.comms_serial_ports_cb.addItems(self._ports)

    @QtCore.pyqtSlot()
    def toggle_obc_serial_connection(self):
        if self.obc_serial_obc.connected:
            self.obc_serial_obc.stop()
        else:
            self.obc_serial_obc.start(str(self.obc_serial_ports_cb.currentText()))

        self._update_obc_serial_connect_btn()
        
        if self._cmd_interface.id == obc_base.OBCBase.InterfaceType.OBC_SERIAL.id:
            self.conn_state_changed.emit(self.obc.connected)

    def _update_obc_serial_connect_btn(self):
        if self.obc_serial_obc.connected:
            self.connect_obc_serial_btn.setText("Connected")
            self.connect_obc_serial_btn.setStyleSheet(ui_utils.Color.GREEN.as_stylesheet("background-color"))
            self.obc_serial_ports_cb.setEnabled(False)
        else:
            self.connect_obc_serial_btn.setText("Connect")
            self.connect_obc_serial_btn.setStyleSheet(None)
            self.obc_serial_ports_cb.setEnabled(True)

    @QtCore.pyqtSlot()
    def toggle_comms_serial_connection(self):
        if self.comms_serial_obc.connected:
            self.comms_serial_obc.stop()
        else:
            self.comms_serial_obc.start(str(self.comms_serial_ports_cb.currentText()))

        self._update_comms_serial_connect_btn()

        if self._cmd_interface.id == obc_base.OBCBase.InterfaceType.COMMS_SERIAL.id:
            self.conn_state_changed.emit(self.obc.connected)

    def _update_comms_serial_connect_btn(self):
        if self.comms_serial_obc.connected:
            self.connect_comms_serial_btn.setText("Connected")
            self.connect_comms_serial_btn.setStyleSheet(ui_utils.Color.GREEN.as_stylesheet("background-color"))
            self.comms_serial_ports_cb.setEnabled(False)
        else:
            self.connect_comms_serial_btn.setText("Connect")
            self.connect_comms_serial_btn.setStyleSheet(None)
            self.comms_serial_ports_cb.setEnabled(True)

    @QtCore.pyqtSlot()
    def use_obc_serial_interface(self) -> None:
        prev_cmd_interface = self._cmd_interface
        self._cmd_interface = obc_base.OBCBase.InterfaceType.OBC_SERIAL
        self._update_cmd_interface(prev_cmd_interface)

    @QtCore.pyqtSlot()
    def use_comms_serial_interface(self) -> None:
        prev_cmd_interface = self._cmd_interface
        self._cmd_interface = obc_base.OBCBase.InterfaceType.COMMS_SERIAL
        self._update_cmd_interface(prev_cmd_interface)

    def _update_cmd_interface(self, prev_cmd_interface: obc_base.OBCBase.InterfaceType = None):
        for btn in self._if_button_map.values():
            btn.setStyleSheet("")

        self._if_button_map[self._cmd_interface.id].setStyleSheet(ui_utils.Color.BLUE.as_stylesheet("background-color"))

        # Send event
        self.obc_cmd_interface_changed.emit((prev_cmd_interface, self._cmd_interface))
