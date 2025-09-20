from PyQt5 import QtWidgets, QtCore

from alea.sa.backend import ttcqt
from alea.sa.ui import control_panels_ui
from alea.sa.util import ui_utils

from alea.sa.widget import cmds_panel
from alea.sa.widget import tasks_panel
from alea.sa.widget import camera_panel
from alea.sa.widget import gps_panel
from alea.sa.widget import eps_panel
from alea.sa.widget import eps_graph_panel
from alea.sa.widget import hil_test_panel

class ControlPanels(QtWidgets.QWidget, control_panels_ui.Ui_ControlPanels):
    TAB_CMDS   = 0
    TAB_TASKS  = 1
    TAB_CAMERA = 2
    TAB_GPS    = 3
    TAB_HIL    = 4

    def __init__(self, obc_provider: ttcqt.OBCTTCInterfaceProvider, parent=None):
        super().__init__(parent)

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.control_tabs: QtWidgets.QTabWidget

        self.setupUi(self)

        self.control_tabs.setCurrentIndex(0)

        # Commands Panel
        self.cmds_panel = cmds_panel.CmdsPanel(obc_provider)
        self.control_tabs.addTab(self.cmds_panel, "Commands")

        # Tasks Panel
        self.tasks_panel = tasks_panel.TasksPanel(obc_provider)
        self.tasks_panel.set_alert.connect(self.handle_tasks_set_alert)
        self.tasks_panel.clear_alert.connect(self.handle_tasks_clear_alert)
        self.control_tabs.addTab(self.tasks_panel, "Tasks")

        # Camera Panel
        self.camera_panel = camera_panel.CameraPanel(obc_provider)
        self.control_tabs.addTab(self.camera_panel, "Camera")

        # GPS Panel
        self.gps_panel = gps_panel.GPSPanel(obc_provider)
        self.control_tabs.addTab(self.gps_panel, "GPS")

        # EPS Panel
        self.eps_panel = eps_panel.EPSPanel(obc_provider)
        self.control_tabs.addTab(self.eps_panel, "EPS Controls")

        # EPS Graph Panel
        self.eps_graph_panel = eps_graph_panel.EPSGraphPanel(obc_provider)
        self.control_tabs.addTab(self.eps_graph_panel, "EPS Graphs")

        # HIL Test Panel
        self.hiltest_panel = hil_test_panel.HILTestsPanel(obc_provider)
        self.control_tabs.addTab(self.hiltest_panel, "HIL Tests")

    @QtCore.pyqtSlot()
    def handle_tasks_set_alert(self):
        self.control_tabs.tabBar().setTabTextColor(self.TAB_TASKS, ui_utils.Color.LIGHT_RED.as_qcolor())

    @QtCore.pyqtSlot()
    def handle_tasks_clear_alert(self):
        self.control_tabs.tabBar().setTabTextColor(self.TAB_TASKS, ui_utils.Color.WHITE.as_qcolor())
