from PyQt5 import QtWidgets, QtCore

from obcpy.utils import serial as serial_utils

from sanantonio.backend import obcqt
from sanantonio.ui import sat_interface_ui
from sanantonio.utils import ui as ui_utils

class SatInterface(QtWidgets.QWidget, sat_interface_ui.Ui_SatInterface):
    obc_serial_conn_state_changed = QtCore.pyqtSignal(bool)

    def __init__(self, obc: obcqt.OBCQT, parent=None):
        super().__init__(parent)

        self._obc = obc
        self._ports = []

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.obc_serial_refresh_btn: QtWidgets.QPushButton
        self.obc_serial_connect_btn: QtWidgets.QPushButton
        self.obc_serial_ports_cb: QtWidgets.QComboBox
        self.obc_serial_status_label: QtWidgets.QLabel

        self.setupUi(self)

        # Connect signals / slots
        self.obc_serial_refresh_btn.clicked.connect(self.refresh_ports)
        self.obc_serial_connect_btn.clicked.connect(self.toggle_connection)

        # Update UI
        self.refresh_ports()
        self.update_connection_state()

    @QtCore.pyqtSlot()
    def refresh_ports(self) -> None:
        # Remove all current ports from combo box
        for i in range(self.obc_serial_ports_cb.count()):
            self.obc_serial_ports_cb.removeItem(i)

        # Get new ports
        new_ports = serial_utils.get_serial_ports()
        self._ports = list(dict.fromkeys(new_ports))

        # Update combo box
        self.obc_serial_ports_cb.addItems(self._ports)

    @QtCore.pyqtSlot()
    def toggle_connection(self):
        if self._obc.connected:
            self._obc.stop()
            self.obc_serial_conn_state_changed.emit(False)
        else:
            self._obc.start(str(self.obc_serial_ports_cb.currentText()))
            self.obc_serial_conn_state_changed.emit(True)
        self.update_connection_state()

    def update_connection_state(self):
        if self._obc.connected:
            self.obc_serial_status_label.setText("Connected")
            self.obc_serial_status_label.setStyleSheet(ui_utils.Color.GREEN.as_stylesheet())
            self.obc_serial_connect_btn.setText("Disconnect")
        else:
            self.obc_serial_status_label.setText("Disconnected")
            self.obc_serial_status_label.setStyleSheet(ui_utils.Color.RED.as_stylesheet())
            self.obc_serial_connect_btn.setText("Connect")
