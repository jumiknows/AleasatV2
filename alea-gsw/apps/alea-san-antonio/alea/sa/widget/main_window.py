from PyQt5 import QtWidgets, QtGui, QtCore
from PyQt5.QtWidgets import QAction

from alea.ttc import ttc_base
from alea.ttc import cmd_sys

from alea.sa.backend import ttcqt
from alea.sa.ui import main_window_ui
from alea.sa import graphics
from alea.sa.util import console
from alea.sa.util import session_manager

from alea.sa.widget import image_label
from alea.sa.widget import san_antonio_log
from alea.sa.widget import sat_interface
from alea.sa.widget import obc_serial_log
from alea.sa.widget import control_panels
from alea.sa.widget import obc_cmd_resp_panel

class MainWindow(QtWidgets.QMainWindow, main_window_ui.Ui_MainWindow, ttc_base.OBCEventListener):
    """Top-level San Antonio window
    """

    stdout_written = QtCore.pyqtSignal(str)
    cmd_sent       = QtCore.pyqtSignal(cmd_sys.OBCCmd)
    resp_recvd     = QtCore.pyqtSignal(object)
    cmd_error      = QtCore.pyqtSignal(object)

    def __init__(self, ttcs: dict[int, ttcqt.TTCQT], parent=None):
        super().__init__(parent)

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.gridLayout: QtWidgets.QGridLayout

        self.setupUi(self)

        self.session_manager = session_manager.SessionManager(self)

        # Create menu bar
        menubar = self.menuBar()

        # Create a menu
        file_menu = menubar.addMenu('Session')

        # Create an action
        save_session_action = QAction('Save Session', self)
        save_session_action.setShortcut('Ctrl+S')
        save_session_action.triggered.connect(self.session_manager.save_session_helper)

        load_session_action = QAction('Load Session', self)
        load_session_action.setShortcut('Ctrl+L')
        load_session_action.triggered.connect(self.session_manager.load_session_helper)

        clear_session_action = QAction('Clear Session', self)
        clear_session_action.setShortcut('Ctrl+N')
        clear_session_action.setShortcut('Ctrl+R')
        clear_session_action.triggered.connect(self.session_manager.new_session_helper)

        # Add action to the menu
        file_menu.addAction(save_session_action)
        file_menu.addAction(load_session_action)
        file_menu.addAction(clear_session_action)

        self.setWindowTitle("San Antonio")
        with graphics.get_path("ALEASAT_icon.png") as icon_path:
            self.setWindowIcon(QtGui.QIcon(str(icon_path.resolve())))

        with graphics.get_path("ALEASAT_logo.png") as logo_path:
            self.logo = image_label.ImageLabel(str(logo_path.resolve()), self.centralwidget)
        self.gridLayout.addWidget(self.logo, 0, 0, 1, 1)

        self.sat_interface = sat_interface.SatInterface(ttcs, self.centralwidget)
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
        self.setMinimumSize(800, 600)

        # # Connect signals / slots
        self.sat_interface.obc_conn_state_changed.connect(self.handle_obc_conn_state_changed)
        self.sat_interface.obc_interface_changed.connect(self.handle_obc_interface_changed)
        self.cmd_sent.connect(self.obc_cmd_resp_panel.handle_cmd_sent)
        self.resp_recvd.connect(self.obc_cmd_resp_panel.handle_resp_recvd)
        self.cmd_error.connect(self.obc_cmd_resp_panel.handle_cmd_error)
        self.stdout_written.connect(self.san_antonio_log.handle_stdout)
        self.control_panels.camera_panel.captured_image.connect(self.obc_cmd_resp_panel.image_panel.handle_image)

        self._ttc_error_connection: QtCore.QMetaObject.Connection = None

    @property
    def obc_ttc(self) -> ttcqt.TTCQT:
        return self.sat_interface.obc_ttc

    def closeEvent(self, a0: QtGui.QCloseEvent) -> None:
        """Disconnect from OBC when the window closes
        """
        self.obc_ttc.stop()

    def cmd_event(self, cmd: cmd_sys.OBCCmd):
        self.cmd_sent.emit(cmd)

    def resp_event(self, resp: cmd_sys.OBCResponse | cmd_sys.OBCPendingResponse):
        self.resp_recvd.emit(resp)

    def error_event(self, cmd: cmd_sys.OBCCmd, error: Exception):
        self.cmd_error.emit((cmd, error))

    @QtCore.pyqtSlot(Exception)
    def handle_ttc_error(self, error: Exception):
        console.print_err(str(error))

    @QtCore.pyqtSlot(bool)
    def handle_obc_conn_state_changed(self, connected: bool):
        if connected:
            self._obc_error_connection = self.obc_ttc.error.connect(self.handle_ttc_error)
            self.obc_ttc.add_obc_event_listener(self)
        else:
            if self._obc_error_connection is not None:
                self.obc_ttc.error.disconnect(self._obc_error_connection)
                self._obc_error_connection = None

            try:
                self.obc_ttc.remove_obc_event_listener(self)
            except ValueError:
                # Raised if event listener was not previously attached, can ignore
                pass

    @QtCore.pyqtSlot(object)
    def handle_obc_interface_changed(self, args: tuple[ttc_base.TTCBase.InterfaceType, ttc_base.TTCBase.InterfaceType]):
        # Deal with previous interface
        if args[0] is not None:
            prev_cmd_obc = self.sat_interface.get_ttc_interface(args[0])
            if prev_cmd_obc.connected:
                prev_cmd_obc.error.disconnect(self._obc_error_connection)

                try:
                    self.obc_ttc.remove_obc_event_listener(self)
                except ValueError:
                    # Raised if event listener was not previously attached, can ignore
                    pass

        # New interface
        if args[1] is not None:
            new_cmd_obc = self.sat_interface.get_ttc_interface(args[1])
            if new_cmd_obc.connected:
                self._obc_error_connection = self.obc_ttc.error.connect(self.handle_ttc_error)
                self.obc_ttc.add_obc_event_listener(self)
