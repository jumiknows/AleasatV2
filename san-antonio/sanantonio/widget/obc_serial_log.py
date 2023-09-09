from PyQt5 import QtWidgets, QtCore

from obcpy.obc.protocol.app import app_log
from obcpy.utils import data as data_utils

from sanantonio.backend import obcqt
from sanantonio.ui import obc_serial_log_ui

PRINTF_LOG_ID = 0

class OBCSerialLog(QtWidgets.QWidget, obc_serial_log_ui.Ui_OBCSerialLog):
    MAX_LOGS = 256

    def __init__(self, obc_provider: obcqt.OBCInterfaceProvider, parent=None):
        super().__init__(parent)

        self._obc_provider = obc_provider

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.obc_log_table: QtWidgets.QTableWidget

        self.setupUi(self)

        header_labels = ["Timestamp", "Level", "Log", "Signal", "Data"]
        self.obc_log_table.setColumnCount(len(header_labels))
        self.obc_log_table.setHorizontalHeaderLabels(header_labels)
        self.obc_log_table.horizontalHeader().setSectionResizeMode(0, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_log_table.horizontalHeader().setSectionResizeMode(1, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_log_table.horizontalHeader().setSectionResizeMode(2, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_log_table.horizontalHeader().setSectionResizeMode(3, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)

        # Connect signals / slots
        self._obc_provider.conn_state_changed.connect(self.handle_conn_state_changed)

        self._logs_connection: QtCore.QMetaObject.Connection = None

    @property
    def obc(self) -> obcqt.OBCQT:
        return self._obc_provider.obc

    @QtCore.pyqtSlot(bool)
    def handle_conn_state_changed(self, connected: bool):
        if connected:
            self._logs_connection = self.obc.logs.connect(self.handle_log)
        else:
            if self._logs_connection is not None:
                self.obc.logs.disconnect(self._logs_connection)
                self._logs_connection = None

    @QtCore.pyqtSlot(app_log.OBCLog)
    def handle_log(self, log: app_log.OBCLog):
        if self.obc_log_table.rowCount() == self.MAX_LOGS:
            self.obc_log_table.removeRow(0)

        new_idx = self.obc_log_table.rowCount()

        self.obc_log_table.insertRow(new_idx)

        self.obc_log_table.setItem(new_idx, 0, QtWidgets.QTableWidgetItem(str(log.date_time)))
        self.obc_log_table.setItem(new_idx, 1, QtWidgets.QTableWidgetItem(str(log.signal_level.name)))
        self.obc_log_table.setItem(new_idx, 2, QtWidgets.QTableWidgetItem(f"{log.group_name} ({log.log_id})"))
        self.obc_log_table.setItem(new_idx, 3, QtWidgets.QTableWidgetItem(f"{log.signal_name} ({log.signal_id})"))
        if log.log_id == PRINTF_LOG_ID:
            self.obc_log_table.setItem(new_idx, 4, QtWidgets.QTableWidgetItem(f"{bytes(log.data['message']).decode()}"))
        else:
            self.obc_log_table.setItem(new_idx, 4, QtWidgets.QTableWidgetItem(f"{log.data_as_string()}"))

        self.obc_log_table.scrollToBottom()
