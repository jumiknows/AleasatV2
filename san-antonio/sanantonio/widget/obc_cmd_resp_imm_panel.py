from typing import Dict, Tuple

from PyQt5 import QtWidgets, QtCore

from obcpy import cmd_sys
from obcpy.utils import exc

from sanantonio.backend import obcqt
from sanantonio.ui import obc_cmd_resp_imm_panel_ui
from sanantonio.utils import console as console_utils
from sanantonio.utils import ui as ui_utils

from . import obc_cmd_resp_base_panel

class OBCCmdRespImmPanel(QtWidgets.QWidget, obc_cmd_resp_imm_panel_ui.Ui_OBCCmdRespImmPanel, obc_cmd_resp_base_panel.OBCCmdRespBasePanel):
    updated = QtCore.pyqtSignal()

    def __init__(self, obc: obcqt.OBCQT, parent=None):
        super().__init__(parent)

        self._obc = obc

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.obc_cmd_resp_imm_table: QtWidgets.QTableWidget

        self.setupUi(self)

        header_labels = ["Inst ID", "CMD ID", "Arguments", "Response Code", "Exec Timestamp", "Response Data", "Error"]
        self.obc_cmd_resp_imm_table.setColumnCount(len(header_labels))
        self.obc_cmd_resp_imm_table.setHorizontalHeaderLabels(header_labels)
        self.obc_cmd_resp_imm_table.horizontalHeader().setDefaultAlignment(QtCore.Qt.AlignmentFlag.AlignLeft)
        self.obc_cmd_resp_imm_table.horizontalHeader().setSectionResizeMode(0, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_cmd_resp_imm_table.horizontalHeader().setSectionResizeMode(1, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_cmd_resp_imm_table.horizontalHeader().setSectionResizeMode(2, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_cmd_resp_imm_table.horizontalHeader().setSectionResizeMode(3, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_cmd_resp_imm_table.horizontalHeader().setSectionResizeMode(4, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_cmd_resp_imm_table.horizontalHeader().setSectionResizeMode(5, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_cmd_resp_imm_table.horizontalHeader().setSectionResizeMode(6, QtWidgets.QHeaderView.ResizeMode.Stretch)

        self.obc_cmd_resp_imm_table.setStyleSheet("QTableWidget::item { padding: 8px }")

        self._pending_cmds: Dict = {int: int} # Maps cmd unique IDs to rows

    @QtCore.pyqtSlot(cmd_sys.cmd.OBCCmd)
    def handle_cmd_sent(self, cmd: cmd_sys.cmd.OBCCmd):
        row_idx = self.obc_cmd_resp_imm_table.rowCount()

        if cmd.cmd_sys_spec.resp is not None:
            # Only waiting for a response if one was specified
            self._pending_cmds[cmd.inst_id] = row_idx

        self.obc_cmd_resp_imm_table.insertRow(row_idx)

        self.obc_cmd_resp_imm_table.setItem(row_idx, 0, QtWidgets.QTableWidgetItem(str(cmd.inst_id)))
        self.obc_cmd_resp_imm_table.setItem(row_idx, 1, QtWidgets.QTableWidgetItem(f"{cmd.cmd_sys_spec.name} ({cmd.cmd_sys_spec.id})"))
        self.obc_cmd_resp_imm_table.setItem(row_idx, 2, QtWidgets.QTableWidgetItem(self.format_args_str(cmd)))

        if cmd.cmd_sys_spec.resp is None:
            self.obc_cmd_resp_imm_table.setItem(row_idx, 3, QtWidgets.QTableWidgetItem(self.NO_DATA))
            self.obc_cmd_resp_imm_table.setItem(row_idx, 4, QtWidgets.QTableWidgetItem(self.NO_DATA))
            self.obc_cmd_resp_imm_table.setItem(row_idx, 5, QtWidgets.QTableWidgetItem(self.NO_DATA))

        self.obc_cmd_resp_imm_table.resizeRowToContents(row_idx)
        self.obc_cmd_resp_imm_table.scrollToItem(self.obc_cmd_resp_imm_table.item(row_idx, 0), QtWidgets.QAbstractItemView.ScrollHint.EnsureVisible)

        self.updated.emit()

    @QtCore.pyqtSlot(cmd_sys.resp.OBCResponse)
    def handle_resp_recvd(self, resp: cmd_sys.resp.OBCResponse):
        try:
            row_idx = self._pending_cmds[resp.cmd_inst_id]
            del self._pending_cmds[resp.cmd_inst_id]
        except KeyError:
            console_utils.print_err(f"Unexpected response: cmd_inst_id = {resp.cmd_inst_id}")
            return

        self.obc_cmd_resp_imm_table.setItem(row_idx, 3, QtWidgets.QTableWidgetItem(resp.data_header.code.name))
        self.obc_cmd_resp_imm_table.setItem(row_idx, 4, QtWidgets.QTableWidgetItem(str(resp.data_header.exec_datetime)))
        self.obc_cmd_resp_imm_table.setItem(row_idx, 5, QtWidgets.QTableWidgetItem(self.format_resp_data_str(resp)))

        self.obc_cmd_resp_imm_table.resizeRowToContents(row_idx)
        self.obc_cmd_resp_imm_table.scrollToItem(self.obc_cmd_resp_imm_table.item(row_idx, 0), QtWidgets.QAbstractItemView.ScrollHint.EnsureVisible)

        self.updated.emit()

    @QtCore.pyqtSlot(object)
    def handle_cmd_error(self, arg: Tuple[cmd_sys.cmd.OBCCmd, exc.OBCError]):
        if arg[0].inst_id in self._pending_cmds:
            # Error corresponds to a pending command
            row_idx = self._pending_cmds[arg[0].inst_id]
            del self._pending_cmds[arg[0].inst_id]

            table_item = QtWidgets.QTableWidgetItem(str(arg[1]))
            table_item.setForeground(ui_utils.Color.RED.as_qcolor())
            self.obc_cmd_resp_imm_table.setItem(row_idx, 6, table_item)

            self.obc_cmd_resp_imm_table.resizeRowToContents(row_idx)
            self.obc_cmd_resp_imm_table.scrollToItem(self.obc_cmd_resp_imm_table.item(row_idx, 6), QtWidgets.QAbstractItemView.ScrollHint.EnsureVisible)

            self.updated.emit()
        else:
            console_utils.print_err(f"Unexpected cmd error for cmd {arg[0].inst_id}: {str(arg[1])}")
