from PyQt5 import QtWidgets

from sanantonio.backend import obcqt
from sanantonio.ui import control_panels_ui
from sanantonio.widget import cmds_panel
from sanantonio.widget import camera_panel
from sanantonio.widget import gps_panel

class ControlPanels(QtWidgets.QWidget, control_panels_ui.Ui_ControlPanels):
    def __init__(self, obc_provider: obcqt.OBCInterfaceProvider, parent=None):
        super().__init__(parent)

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.control_tabs: QtWidgets.QTabWidget

        self.setupUi(self)

        self.control_tabs.setCurrentIndex(0)

        # Commands Panel
        self.cmds_panel = cmds_panel.CmdsPanel(obc_provider)
        self.control_tabs.addTab(self.cmds_panel, "Commands")

        # Camera Panel
        self.camera_panel = camera_panel.CameraPanel(obc_provider)
        self.control_tabs.addTab(self.camera_panel, "Camera")

        # GPS Panel
        self.gps_panel = gps_panel.GpsPanel(obc_provider)
        self.control_tabs.addTab(self.gps_panel, "GPS")