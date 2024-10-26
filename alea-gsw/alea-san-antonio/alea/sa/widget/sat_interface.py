from PyQt5 import QtWidgets, QtCore

from alea.ttc import ttc_base
from alea.ttc.protocol.app import app_log
from alea.ttc.device import serial_device

from alea.sa.ui import sat_interface_ui
from alea.sa.backend import ttcqt
from alea.sa.util import ui_utils

class SatInterface(QtWidgets.QWidget, sat_interface_ui.Ui_SatInterface, ttcqt.OBCTTCInterfaceProvider):
    _obc_conn_state_changed = QtCore.pyqtSignal(bool)
    _obc_interface_changed = QtCore.pyqtSignal(object)

    _logs = QtCore.pyqtSignal(app_log.OBCLog)

    def __init__(self, ttcs: dict[int, ttcqt.TTCQT], parent=None):
        super().__init__(parent)

        self._ttcs = ttcs

        self._obc_interface_type = ttc_base.TTCBase.InterfaceType.OBC_SERIAL
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
            ttc_base.TTCBase.InterfaceType.OBC_SERIAL.id   : self.obc_serial_if_btn,
            ttc_base.TTCBase.InterfaceType.COMMS_SERIAL.id : self.comms_serial_if_btn,
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
        self._update_obc_interface()

    # =========================================================================
    # Implementation of OBCTTCInterfaceProvider interface
    # =========================================================================

    @property
    def obc_ttc(self) -> ttcqt.TTCQT:
        # TTC for OBC
        return self._ttcs[self._obc_interface_type.id]

    @property
    def obc_logs(self) -> QtCore.pyqtBoundSignal:
        # Logs are only available on the OBC_SERIAL interface
        return self._ttcs[ttc_base.TTCBase.InterfaceType.OBC_SERIAL.id].logs

    @property
    def obc_conn_state_changed(self) -> QtCore.pyqtBoundSignal:
        return self._obc_conn_state_changed

    @property
    def obc_interface_changed(self) -> QtCore.pyqtBoundSignal:
        return self._obc_interface_changed

    # =========================================================================

    @property
    def obc_serial_ttc(self) -> ttcqt.TTCQT:
        return self._ttcs[ttc_base.TTCBase.InterfaceType.OBC_SERIAL.id]

    @property
    def comms_serial_ttc(self) -> ttcqt.TTCQT:
        return self._ttcs[ttc_base.TTCBase.InterfaceType.COMMS_SERIAL.id]

    def get_ttc_interface(self, interface_type: ttc_base.TTCBase.InterfaceType) -> ttcqt.TTCQT:
        return self._ttcs[interface_type.id]

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
        new_ports = serial_device.get_serial_ports()
        self._ports = list(dict.fromkeys(new_ports))

        # Update combo boxes
        self.obc_serial_ports_cb.addItems(self._ports)
        self.comms_serial_ports_cb.addItems(self._ports)

    @QtCore.pyqtSlot()
    def toggle_obc_serial_connection(self):
        if self.obc_serial_ttc.connected:
            self.obc_serial_ttc.stop()
        else:
            self.obc_serial_ttc.start(str(self.obc_serial_ports_cb.currentText()))

        self._update_obc_serial_connect_btn()

        if self._obc_interface_type.id == ttc_base.TTCBase.InterfaceType.OBC_SERIAL.id:
            self.obc_conn_state_changed.emit(self.obc_ttc.connected)

    def _update_obc_serial_connect_btn(self):
        if self.obc_serial_ttc.connected:
            self.connect_obc_serial_btn.setText("Connected")
            self.connect_obc_serial_btn.setStyleSheet(ui_utils.Color.GREEN.as_stylesheet("background-color"))
            self.obc_serial_ports_cb.setEnabled(False)
        else:
            self.connect_obc_serial_btn.setText("Connect")
            self.connect_obc_serial_btn.setStyleSheet(None)
            self.obc_serial_ports_cb.setEnabled(True)

    @QtCore.pyqtSlot()
    def toggle_comms_serial_connection(self):
        if self.comms_serial_ttc.connected:
            self.comms_serial_ttc.stop()
        else:
            self.comms_serial_ttc.start(str(self.comms_serial_ports_cb.currentText()))

        self._update_comms_serial_connect_btn()

        if self._obc_interface_type.id == ttc_base.TTCBase.InterfaceType.COMMS_SERIAL.id:
            self.obc_conn_state_changed.emit(self.obc_ttc.connected)

    def _update_comms_serial_connect_btn(self):
        if self.comms_serial_ttc.connected:
            self.connect_comms_serial_btn.setText("Connected")
            self.connect_comms_serial_btn.setStyleSheet(ui_utils.Color.GREEN.as_stylesheet("background-color"))
            self.comms_serial_ports_cb.setEnabled(False)
        else:
            self.connect_comms_serial_btn.setText("Connect")
            self.connect_comms_serial_btn.setStyleSheet(None)
            self.comms_serial_ports_cb.setEnabled(True)

    @QtCore.pyqtSlot()
    def use_obc_serial_interface(self) -> None:
        prev_interface = self._obc_interface_type
        self._obc_interface_type = ttc_base.TTCBase.InterfaceType.OBC_SERIAL
        self._update_obc_interface(prev_interface)

    @QtCore.pyqtSlot()
    def use_comms_serial_interface(self) -> None:
        prev_interface = self._obc_interface_type
        self._obc_interface_type = ttc_base.TTCBase.InterfaceType.COMMS_SERIAL
        self._update_obc_interface(prev_interface)

    def _update_obc_interface(self, prev_interface: ttc_base.TTCBase.InterfaceType = None):
        for btn in self._if_button_map.values():
            btn.setStyleSheet("")

        self._if_button_map[self._obc_interface_type.id].setStyleSheet(ui_utils.Color.BLUE.as_stylesheet("background-color"))

        # Send event
        self.obc_interface_changed.emit((prev_interface, self._obc_interface_type))
