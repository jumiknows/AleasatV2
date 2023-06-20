from typing import Union, Dict
import pathlib

from PyQt5 import QtWidgets, QtGui, QtCore

from obcpy import cmd_sys
from obcpy.obc import obc_base
from obcpy.utils import exc

from sanantonio.backend import obcqt

from sanantonio.ui import main_window_ui

from sanantonio.widget import sat_interface
from sanantonio.widget import control_panels
from sanantonio.widget import obc_cmd_resp_panel
from sanantonio.widget import san_antonio_log
from sanantonio.widget import obc_serial_log
from sanantonio.widget import image_label

from sanantonio.utils import console as console_utils

GRAPHICS_DIR = pathlib.Path(__file__).resolve().parent.parent / "graphics"
ALEASAT_LOGO_PATH = GRAPHICS_DIR / "ALEASAT_logo.png"
ALEASAT_ICON_PATH = GRAPHICS_DIR / "ALEASAT_icon.png"

class MainWindow(QtWidgets.QMainWindow, main_window_ui.Ui_MainWindow, obc_base.OBCEventListener):
    """Top-level San Antonio window
    """

    stdout_written = QtCore.pyqtSignal(str)
    cmd_sent = QtCore.pyqtSignal(cmd_sys.cmd.OBCCmd)
    resp_recvd = QtCore.pyqtSignal(object)
    cmd_error = QtCore.pyqtSignal(object)

    def __init__(self, obcs: Dict[str, obcqt.OBCQT], parent=None):
        super().__init__(parent)

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.gridLayout: QtWidgets.QGridLayout

        self.setupUi(self)

        self.setWindowTitle("San Antonio")
        self.setWindowIcon(QtGui.QIcon(str(ALEASAT_ICON_PATH)))

        self.logo = image_label.ImageLabel(str(ALEASAT_LOGO_PATH), self.centralwidget)
        self.gridLayout.addWidget(self.logo, 0, 0, 1, 1)

        self.sat_interface = sat_interface.SatInterface(obcs, self.centralwidget)
        self.gridLayout.addWidget(self.sat_interface, 0, 1, 1, 2)

        self.control_panels = control_panels.ControlPanels(self.sat_interface, self.centralwidget)
        self.gridLayout.addWidget(self.control_panels, 1, 0, 1, 3)

        self.obc_cmd_resp_panel = obc_cmd_resp_panel.OBCCmdRespPanel(self.sat_interface, self.centralwidget)
        self.gridLayout.addWidget(self.obc_cmd_resp_panel, 0, 3, 2, 3)

        self.san_antonio_log = san_antonio_log.SanAntonioLog(self.centralwidget)
        self.gridLayout.addWidget(self.san_antonio_log, 2, 0, 1, 3)

        self.obc_serial_log = obc_serial_log.OBCSerialLog(self.sat_interface, self.centralwidget)
        self.gridLayout.addWidget(self.obc_serial_log, 2, 3, 1, 3)

        self.gridLayout.setRowStretch(0, 0)
        self.gridLayout.setRowStretch(1, 2)
        self.gridLayout.setRowStretch(2, 1)

        # Connect signals / slots
        self.sat_interface.conn_state_changed.connect(self.handle_conn_state_changed)
        self.cmd_sent.connect(self.obc_cmd_resp_panel.handle_cmd_sent)
        self.resp_recvd.connect(self.obc_cmd_resp_panel.handle_resp_recvd)
        self.cmd_error.connect(self.obc_cmd_resp_panel.handle_cmd_error)
        self.stdout_written.connect(self.san_antonio_log.handle_stdout)
        self.control_panels.camera_panel.captured_image.connect(self.obc_cmd_resp_panel.image_panel.handle_image)

        self._obc_error_connection: QtCore.QMetaObject.Connection = None

    @property
    def obc(self) -> obcqt.OBCQT:
        return self.sat_interface.obc

    def closeEvent(self, a0: QtGui.QCloseEvent) -> None:
        """Disconnect from OBC when the window closes
        """
        self.obc.stop()

    def cmd_event(self, cmd: cmd_sys.cmd.OBCCmd):
        self.cmd_sent.emit(cmd)

    def resp_event(self, resp: Union[cmd_sys.resp.OBCResponse, cmd_sys.resp.OBCPendingResponse]):
        self.resp_recvd.emit(resp)

    def error_event(self, cmd: cmd_sys.cmd.OBCCmd, error: exc.OBCError):
        self.cmd_error.emit((cmd, error))

    @QtCore.pyqtSlot(exc.OBCError)
    def handle_obc_error(self, error: exc.OBCError):
        console_utils.print_err(str(error))

    @QtCore.pyqtSlot(bool)
    def handle_conn_state_changed(self, connected: bool):
        if connected:
            self._obc_error_connection = self.obc.obc_error.connect(self.handle_obc_error)
            self.obc.add_event_listener(self)
        else:
            if self._obc_error_connection is not None:
                self.obc.obc_error.disconnect(self._obc_error_connection)
                self._obc_error_connection = None

            try:
                self.obc.remove_event_listener(self)
            except ValueError:
                # Raised if event listener was not previously attached, can ignore
                pass
