from typing import Union, Tuple

from PyQt5 import QtWidgets, QtCore

from obcpy import cmd_sys
from obcpy.utils import exc

from sanantonio.backend import obcqt
from sanantonio.ui import obc_cmd_resp_panel_ui
from sanantonio.widget import obc_cmd_resp_imm_panel
from sanantonio.widget import obc_cmd_resp_sched_panel
from sanantonio.widget import image_panel
from sanantonio.utils import console as console_utils

class OBCCmdRespPanel(QtWidgets.QWidget, obc_cmd_resp_panel_ui.Ui_OBCCmdRespPanel):
    TAB_IMMEDIATE = 0
    TAB_SCHEDULED = 1
    TAB_IMAGE     = 2

    imm_cmd_sent = QtCore.pyqtSignal(cmd_sys.cmd.OBCCmd)
    imm_resp_recvd = QtCore.pyqtSignal(cmd_sys.resp.OBCResponse)
    imm_cmd_error = QtCore.pyqtSignal(object)

    sched_cmd_sent = QtCore.pyqtSignal(cmd_sys.cmd.OBCCmd)
    sched_resp_recvd = QtCore.pyqtSignal(cmd_sys.resp.OBCPendingResponse)
    sched_cmd_error = QtCore.pyqtSignal(object)

    def __init__(self, obc: obcqt.OBCQT, parent=None):
        super().__init__(parent)

        self._obc = obc

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.obc_cmd_resp_tabs: QtWidgets.QTabWidget

        self.setupUi(self)

        # Immediate Panel
        self.immediate = obc_cmd_resp_imm_panel.OBCCmdRespImmPanel(self._obc)
        self.obc_cmd_resp_tabs.insertTab(self.TAB_IMMEDIATE, self.immediate, "Immediate")

        # Scheduled Panel
        self.scheduled = obc_cmd_resp_sched_panel.OBCCmdRespSchedPanel(self._obc)
        self.obc_cmd_resp_tabs.insertTab(self.TAB_SCHEDULED, self.scheduled, "Scheduled")

        # Image Panel
        self.image_panel = image_panel.ImagePanel(self._obc)
        self.obc_cmd_resp_tabs.insertTab(self.TAB_IMAGE, self.image_panel, "Camera Image")

        self.obc_cmd_resp_tabs.setCurrentIndex(self.TAB_IMMEDIATE)

        # Connect signals / slots
        self.imm_cmd_sent.connect(self.immediate.handle_cmd_sent)
        self.imm_resp_recvd.connect(self.immediate.handle_resp_recvd)
        self.imm_cmd_error.connect(self.immediate.handle_cmd_error)
        self.immediate.updated.connect(self.handle_imm_updated)

        self.sched_cmd_sent.connect(self.scheduled.handle_cmd_sent)
        self.sched_resp_recvd.connect(self.scheduled.handle_resp_recvd)
        self.sched_cmd_error.connect(self.scheduled.handle_cmd_error)
        self.scheduled.updated.connect(self.handle_sched_updated)

        self.image_panel.updated.connect(self.handle_image_updated)

    @QtCore.pyqtSlot(cmd_sys.cmd.OBCCmd)
    def handle_cmd_sent(self, cmd: cmd_sys.cmd.OBCCmd):
        # Forward the signal to the appropriate panel
        if cmd.date_time.is_immediate:
            self.imm_cmd_sent.emit(cmd)
        else:
            self.sched_cmd_sent.emit(cmd)

    @QtCore.pyqtSlot(object)
    def handle_resp_recvd(self, resp: Union[cmd_sys.resp.OBCResponse, exc.OBCError]):
        # Forward the signal to the appropriate panel
        if isinstance(resp, cmd_sys.resp.OBCResponse):
            self.imm_resp_recvd.emit(resp)
        elif isinstance(resp, cmd_sys.resp.OBCPendingResponse):
            self.sched_resp_recvd.emit(resp)
        elif resp is not None:
            console_utils.print_err(f"Unexpected response object: {type(resp)}")

    @QtCore.pyqtSlot(object)
    def handle_cmd_error(self, arg: Tuple[cmd_sys.cmd.OBCCmd, exc.OBCError]):
        # Forward the signal to the appropriate panel
        if arg[0].date_time.is_immediate:
            self.imm_cmd_error.emit(arg)
        else:
            self.sched_cmd_error.emit(arg)

    @QtCore.pyqtSlot()
    def handle_imm_updated(self):
        # Automatically switch to the panel when it's updated
        self.obc_cmd_resp_tabs.setCurrentIndex(self.TAB_IMMEDIATE)

    @QtCore.pyqtSlot()
    def handle_sched_updated(self):
        # Automatically switch to the panel when it's updated
        self.obc_cmd_resp_tabs.setCurrentIndex(self.TAB_SCHEDULED)

    @QtCore.pyqtSlot()
    def handle_image_updated(self):
        # Automatically switch to the panel when it's updated
        self.obc_cmd_resp_tabs.setCurrentIndex(self.TAB_IMAGE)
