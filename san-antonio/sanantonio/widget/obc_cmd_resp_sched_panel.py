from typing import Dict, Tuple

from PyQt5 import QtWidgets, QtCore

from obcpy import cmd_sys
from obcpy.utils import exc

from sanantonio.backend import obcqt
from sanantonio.ui import obc_cmd_resp_sched_panel_ui
from sanantonio.utils import console as console_utils
from sanantonio.utils import ui as ui_utils

from . import obc_cmd_resp_base_panel

class OBCCmdRespSchedPanel(QtWidgets.QWidget, obc_cmd_resp_sched_panel_ui.Ui_OBCCmdRespSchedPanel, obc_cmd_resp_base_panel.OBCCmdRespBasePanel):
    updated = QtCore.pyqtSignal()

    def __init__(self, obc: obcqt.OBCQT, parent=None):
        super().__init__(parent)

        self._obc = obc

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.obc_cmd_resp_sched_tree: QtWidgets.QTreeWidget

        self.setupUi(self)

        header_labels = ["Inst ID", "CMD ID", "Timestamp", "Arguments", "Response Code", "Exec Timestamp", "Response Data", "Error"]
        self.obc_cmd_resp_sched_tree.setColumnCount(len(header_labels))
        self.obc_cmd_resp_sched_tree.setHeaderLabels(header_labels)
        self.obc_cmd_resp_sched_tree.header().setDefaultAlignment(QtCore.Qt.AlignmentFlag.AlignLeft)
        self.obc_cmd_resp_sched_tree.header().setSectionResizeMode(0, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_cmd_resp_sched_tree.header().setSectionResizeMode(1, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_cmd_resp_sched_tree.header().setSectionResizeMode(2, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_cmd_resp_sched_tree.header().setSectionResizeMode(3, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_cmd_resp_sched_tree.header().setSectionResizeMode(4, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_cmd_resp_sched_tree.header().setSectionResizeMode(5, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_cmd_resp_sched_tree.header().setSectionResizeMode(6, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.obc_cmd_resp_sched_tree.header().setSectionResizeMode(7, QtWidgets.QHeaderView.ResizeMode.Stretch)

        self.obc_cmd_resp_sched_tree.setStyleSheet("QTreeWidget::item { padding: 8px }")

        self._pending_cmds: Dict = {int: int} # Maps cmd unique IDs to rows

    @QtCore.pyqtSlot(cmd_sys.cmd.OBCCmd)
    def handle_cmd_sent(self, cmd: cmd_sys.cmd.OBCCmd):
        row_idx = self.obc_cmd_resp_sched_tree.topLevelItemCount()
        self._pending_cmds[cmd.inst_id] = row_idx

        item = QtWidgets.QTreeWidgetItem(None, [
            str(cmd.inst_id),
            f"{cmd.cmd_sys_spec.name} ({cmd.cmd_sys_spec.id})",
            str(cmd.date_time),
            self.format_args_str(cmd),
        ])

        self.obc_cmd_resp_sched_tree.addTopLevelItem(item)

        self.obc_cmd_resp_sched_tree.scrollToItem(item, QtWidgets.QAbstractItemView.ScrollHint.EnsureVisible)

        self.updated.emit()

    @QtCore.pyqtSlot(cmd_sys.resp.OBCPendingResponse)
    def handle_resp_recvd(self, resp: cmd_sys.resp.OBCPendingResponse):
        try:
            row_idx = self._pending_cmds[resp.sched_resp.cmd_inst_id]
        except KeyError:
            console_utils.print_err(f"Unexpected response: cmd_inst_id = {resp.sched_resp.cmd_inst_id}")
            return

        item = self.obc_cmd_resp_sched_tree.topLevelItem(row_idx)
        item.setData(4, QtCore.Qt.ItemDataRole.DisplayRole, resp.sched_resp.data_header.code.name)
        item.setData(5, QtCore.Qt.ItemDataRole.DisplayRole, str(resp.sched_resp.data_header.exec_datetime))

        self.obc_cmd_resp_sched_tree.scrollToItem(item, QtWidgets.QAbstractItemView.ScrollHint.EnsureVisible)

        # Check if the actual command has no response
        if self._obc.cmd_sys_specs.get(id=resp.sched_resp.cmd_id).resp is None:
            del self._pending_cmds[resp.sched_resp.cmd_inst_id]
        else:
            # Check if the actual response is available
            actual_resp = resp.wait(0)
            if actual_resp is not None:
                # Now that we have the actual response there's nothing left pending
                del self._pending_cmds[resp.sched_resp.cmd_inst_id]

                sub_item = QtWidgets.QTreeWidgetItem(None, [
                    "",
                    "",
                    "",
                    "",
                    actual_resp.data_header.code.name,
                    str(actual_resp.data_header.exec_datetime),
                    self.format_resp_data_str(actual_resp),
                ])
                item.addChild(sub_item)
                item.setExpanded(True)

                self.obc_cmd_resp_sched_tree.scrollToItem(sub_item, QtWidgets.QAbstractItemView.ScrollHint.EnsureVisible)

        self.updated.emit()

    @QtCore.pyqtSlot(object)
    def handle_cmd_error(self, arg: Tuple[cmd_sys.cmd.OBCCmd, exc.OBCError]):
        if arg[0].inst_id in self._pending_cmds:
            # Error corresponds to a pending command
            row_idx = self._pending_cmds[arg[0].inst_id]
            del self._pending_cmds[arg[0].inst_id]

            item = self.obc_cmd_resp_sched_tree.topLevelItem(row_idx)
            item.setData(7, QtCore.Qt.ItemDataRole.DisplayRole, str(arg[1]))
            item.setData(7, QtCore.Qt.ItemDataRole.ToolTipRole, str(arg[1]))
            item.setForeground(7, ui_utils.Color.RED.as_qcolor())

            self.obc_cmd_resp_sched_tree.scrollToItem(item, QtWidgets.QAbstractItemView.ScrollHint.EnsureVisible)

            self.updated.emit()
        else:
            console_utils.print_err(f"Unexpected cmd error for cmd {arg[0].inst_id}: {str(arg[1])}")
