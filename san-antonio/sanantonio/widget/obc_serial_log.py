from typing import Any

from PyQt5 import QtWidgets, QtCore

from obcpy import log_sys
from obcpy.obc.protocol.app import app_log
from obcpy.utils import data as data_utils

from sanantonio.backend import obcqt
from sanantonio.ui import obc_serial_log_ui
from sanantonio.utils import ui as ui_utils
from sanantonio.utils import console as console_utils

import csv

PRINTF_LOG_ID = 0
PRINT_DEBUG = 1

class OBCSerialLog(QtWidgets.QWidget, obc_serial_log_ui.Ui_OBCSerialLog):
    MAX_LOGS = 256

    def __init__(self, obc_provider: obcqt.OBCInterfaceProvider, parent=None):
        super().__init__(parent)

        self._obc_provider = obc_provider

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.obc_log_table: QtWidgets.QTableWidget
        self.obc_log_clear_btn: QtWidgets.QPushButton
        self.obc_log_save_btn: QtWidgets.QPushButton

        self.watchdog_checkbox: QtWidgets.QCheckBox

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
        self.obc_log_clear_btn.clicked.connect(self.handle_clear)
        self.obc_log_save_btn.clicked.connect(self.handle_save)

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
        if not self.watchdog_checkbox.isChecked():
            if (log.signal_name == "SW_WD_HAPPY") or (log.signal_name == "HW_WD_PET"):
                return

        if self.obc_log_table.rowCount() == self.MAX_LOGS:
            self.obc_log_table.removeRow(0)

        new_idx = self.obc_log_table.rowCount()

        self.obc_log_table.insertRow(new_idx)

        text_color = ui_utils.Color.WHITE
        if log.signal_level == log_sys.log_spec.OBCLogLevel.ERROR:
            text_color = ui_utils.Color.RED

        self.obc_log_table.setItem(new_idx, 0, self._create_colored_cell(log.date_time, text_color))
        self.obc_log_table.setItem(new_idx, 1, self._create_colored_cell(log.signal_level.name, text_color))
        self.obc_log_table.setItem(new_idx, 2, self._create_colored_cell(f"{log.group_name} ({log.log_id})", text_color))
        self.obc_log_table.setItem(new_idx, 3, self._create_colored_cell(f"{log.signal_name} ({log.signal_id})", text_color))

        log_data_str = "Error decoding log data"

        if log.log_id == PRINTF_LOG_ID:
            try:
                if log.signal_id == PRINT_DEBUG:
                    log_data_str = f"{bytes(log.data['filename'][:log.data['length']]).decode()}:{log.data['line']}"
                else:
                    log_data_str = f"{bytes(log.data['message']).decode()}"
            except UnicodeDecodeError as e:
                console_utils.print_err(str(e))
        else:
            log_data_str = f"{log.data_as_string()}"

        self.obc_log_table.setItem(new_idx, 4, self._create_colored_cell(log_data_str, text_color))

        self.obc_log_table.scrollToBottom()

    def _create_colored_cell(self, text: Any, color: ui_utils.Color) -> QtWidgets.QTableWidgetItem:
        item = QtWidgets.QTableWidgetItem(str(text))
        item.setForeground(color.as_qcolor())
        return item

    def handle_clear(self):
        self.obc_log_table.clearContents()
        self.obc_log_table.setRowCount(0)

    def handle_save(self):
        file_name, _ = QtWidgets.QFileDialog.getSaveFileName(self, "Save Serial Log", "", "CSV Files (*.csv)")
        print(f"Saving Serial Log table to file: {file_name}")
        if file_name:
            with open(file_name, 'w', newline='') as file:
                writer = csv.writer(file)
                writer.writerow([self.obc_log_table.horizontalHeaderItem(i).text() for i in range(self.obc_log_table.columnCount())])
                for row in range(self.obc_log_table.rowCount()):
                    row_data = []
                    for column in range(self.obc_log_table.columnCount()):
                        item = self.obc_log_table.item(row, column)
                        if item is not None:
                            row_data.append(item.text())
                        else:
                            row_data.append("")
                    writer.writerow(row_data)
