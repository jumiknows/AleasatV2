from PyQt5 import QtWidgets, QtCore
from typing import Dict

from obcpy.utils import serial as serial_utils
from obcpy.obc import obc_base

from sanantonio.backend import obcqt
from sanantonio.ui import sat_interface_ui
from sanantonio.utils import ui as ui_utils

class SatInterface(QtWidgets.QWidget, sat_interface_ui.Ui_SatInterface, obcqt.OBCInterfaceProvider):
    _conn_state_changed = QtCore.pyqtSignal(bool)
    _obc_interface_changed = QtCore.pyqtSignal(obc_base.OBCBase.InterfaceType)

    def __init__(self, obcs: Dict[str, obcqt.OBCQT], parent=None):
        super().__init__(parent)

        self._obcs = obcs

        self._desired_interface = obc_base.OBCBase.InterfaceType.OBC_SERIAL
        self._serial_ports = []

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.obc_serial_if_btn: QtWidgets.QPushButton
        self.obc_serial_ports_cb: QtWidgets.QComboBox

        self.comms_serial_if_btn: QtWidgets.QPushButton
        self.comms_serial_ports_cb: QtWidgets.QComboBox

        self.refresh_devices_btn: QtWidgets.QPushButton

        self.connect_btn: QtWidgets.QPushButton
        self.conn_status_label: QtWidgets.QLabel

        self.setupUi(self)

        self._if_button_map = {
            obc_base.OBCBase.InterfaceType.OBC_SERIAL.id   : self.obc_serial_if_btn,
            obc_base.OBCBase.InterfaceType.COMMS_SERIAL.id : self.comms_serial_if_btn,
        }

        # Connect signals / slots
        self.obc_serial_if_btn.clicked.connect(self.use_obc_serial_interface)
        self.comms_serial_if_btn.clicked.connect(self.use_comms_serial_interface)

        self.refresh_devices_btn.clicked.connect(self.handle_refresh_clicked)
        self.connect_btn.clicked.connect(self.toggle_connection)
    
        # Update UI
        self._refresh_devices()
        self._update_connection_state()
        self._update_selected_interface()

    @property
    def obc(self) -> obcqt.OBCQT:
        return self._obcs[self._desired_interface.id]

    @property
    def conn_state_changed(self) -> QtCore.pyqtBoundSignal:
        return self._conn_state_changed

    @property
    def obc_interface_changed(self) -> QtCore.pyqtBoundSignal:
        return self._obc_interface_changed

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
    def toggle_connection(self):
        if self.obc.connected:
            self.obc.stop()
        else:
            if self._desired_interface == obc_base.OBCBase.InterfaceType.OBC_SERIAL:
                self.obc.start(str(self.obc_serial_ports_cb.currentText()))
            elif self._desired_interface == obc_base.OBCBase.InterfaceType.COMMS_SERIAL:
                self.obc.start(str(self.comms_serial_ports_cb.currentText()))

        self._update_connection_state()

    def _update_connection_state(self):
        if self.obc.connected:
            self.conn_status_label.setText("Connected")
            self.conn_status_label.setStyleSheet(ui_utils.Color.GREEN.as_stylesheet())
            self.connect_btn.setText("Disconnect")

            self.obc_serial_ports_cb.setDisabled(True)
            self.comms_serial_ports_cb.setDisabled(True)
            self.refresh_devices_btn.setDisabled(True)

            for interface_id in self._if_button_map:
                btn = self._if_button_map[interface_id]
                btn.setEnabled(False)

                if interface_id == self._desired_interface.id:
                    btn.setFlat(False)
                    btn.setStyleSheet(ui_utils.Color.GREEN.as_stylesheet("background-color"))
                else:
                    btn.setFlat(True)
                    btn.setStyleSheet("")
        else:
            self.conn_status_label.setText("Disconnected")
            self.conn_status_label.setStyleSheet(ui_utils.Color.RED.as_stylesheet())
            self.connect_btn.setText("Connect")

            self.obc_serial_ports_cb.setDisabled(False)
            self.comms_serial_ports_cb.setDisabled(False)
            self.refresh_devices_btn.setDisabled(False)

            for interface_id in self._if_button_map:
                btn = self._if_button_map[interface_id]
                btn.setEnabled(True)
                btn.setFlat(False)

                if interface_id == self._desired_interface.id:
                    btn.setStyleSheet(ui_utils.Color.RED.as_stylesheet("background-color"))
                else:
                    btn.setStyleSheet("")

        self.conn_state_changed.emit(self.obc.connected)

    @QtCore.pyqtSlot()
    def use_obc_serial_interface(self) -> None:
        self._desired_interface = obc_base.OBCBase.InterfaceType.OBC_SERIAL
        self._update_selected_interface()

    @QtCore.pyqtSlot()
    def use_comms_serial_interface(self) -> None:
        self._desired_interface = obc_base.OBCBase.InterfaceType.COMMS_SERIAL
        self._update_selected_interface()

    def _update_selected_interface(self):
        for btn in self._if_button_map.values():
            btn.setStyleSheet("")

        self._if_button_map[self._desired_interface.id].setStyleSheet(ui_utils.Color.RED.as_stylesheet("background-color"))

        self.obc_interface_changed.emit(self._desired_interface)
        