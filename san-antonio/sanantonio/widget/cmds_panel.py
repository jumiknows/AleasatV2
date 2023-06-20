from PyQt5 import QtWidgets, QtCore

from obcpy.utils import exc
from obcpy.utils import obc_time

from sanantonio.backend import obcqt
from sanantonio.ui.control_panels import cmds_panel_ui

class CmdsPanel(QtWidgets.QWidget, cmds_panel_ui.Ui_CmdsPanel):
    def __init__(self, obc_provider: obcqt.OBCInterfaceProvider, parent=None):
        super().__init__(parent)

        self._obc_provider = obc_provider

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.reset_btn: QtWidgets.QPushButton
        self.ping_btn: QtWidgets.QPushButton
        self.sync_time_btn: QtWidgets.QPushButton

        self.immediate_radio_btn: QtWidgets.QRadioButton
        self.scheduled_radio_btn: QtWidgets.QRadioButton
        self.custom_cmd_now_btn: QtWidgets.QPushButton
        self.date_time_edit: QtWidgets.QDateTimeEdit
        self.cmd_name_edit: QtWidgets.QLineEdit
        self.cmd_args_edit: QtWidgets.QLineEdit
        self.send_btn: QtWidgets.QPushButton

        self.custom_cmd_help_label: QtWidgets.QLabel

        self.setupUi(self)

        cmd_names = list(map(lambda cmd_sys_spec: cmd_sys_spec.name, self.obc.cmd_sys_specs))
        cmd_name_completer = QtWidgets.QCompleter(cmd_names, self)
        cmd_name_completer.setCaseSensitivity(QtCore.Qt.CaseSensitivity.CaseInsensitive)
        cmd_name_completer.setCompletionMode(QtWidgets.QCompleter.CompletionMode.PopupCompletion)
        self.cmd_name_edit.setCompleter(cmd_name_completer)

        # Connect signals / slots
        self.reset_btn.clicked.connect(self.handle_reset)
        self.ping_btn.clicked.connect(self.handle_ping)
        self.sync_time_btn.clicked.connect(self.handle_sync_time)
        self.send_btn.clicked.connect(self.handle_send)
        self.cmd_name_edit.returnPressed.connect(self.handle_send)
        self.cmd_args_edit.returnPressed.connect(self.handle_send)
        self.cmd_name_edit.textChanged.connect(self.handle_cmd_name_changed)
        self.custom_cmd_now_btn.clicked.connect(self.handle_now_clicked)

    @property
    def obc(self) -> obcqt.OBCQT:
        return self._obc_provider.obc

    @QtCore.pyqtSlot()
    def handle_reset(self):
        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.reset()
        ))

    @QtCore.pyqtSlot()
    def handle_ping(self):
        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.ping()
        ))

    @QtCore.pyqtSlot()
    def handle_sync_time(self):
        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.set_time()
        ))

    @QtCore.pyqtSlot()
    def handle_now_clicked(self):
        self.date_time_edit.setDateTime(QtCore.QDateTime.currentDateTime())

    @QtCore.pyqtSlot(str)
    def handle_cmd_name_changed(self, cmd_name: str):
        try:
            cmd_sys_spec = self.obc.cmd_sys_specs.get(cmd_name)
            self.custom_cmd_help_label.setText(str(cmd_sys_spec))
        except exc.OBCCmdNotFoundError:
            self.custom_cmd_help_label.setText("")

    @QtCore.pyqtSlot()
    def handle_send(self):
        cmd_name = self.cmd_name_edit.text()
        cmd_args = self.cmd_args_edit.text()

        cmd_str = cmd_name
        if len(cmd_args) > 0:
            cmd_str += " "
            cmd_str += cmd_args

        if self.scheduled_radio_btn.isChecked():
            cmd_date_time = obc_time.OBCDateTime(self.date_time_edit.dateTime().toPyDateTime())
        else:
            cmd_date_time = obc_time.IMMEDIATE

        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc, cmd_str=cmd_str: obc.send_cmd_str(cmd_str, date_time = cmd_date_time)
        ))
