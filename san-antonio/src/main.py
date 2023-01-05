from typing import Union
import sys
import globs
from pathlib import Path

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtGui import QColor
from PyQt5.QtWidgets import QApplication, QShortcut

from obcpy.obc import OBC
from obcpy.cmd_sys.resp import OBCResponse
from obcpy.utils.exc import OBCError, OBCCmdNotFoundError
from obcpy.utils.serial import get_serial_ports
from obcpy.utils.data import bytes_to_hexstr
from obcpy.obc_protocol.log import OBCLog

import san_antonio, history, config, constants
from obcqt import OBCQT, OBCQTRequest

CMDS_PATHS = [
    Path(__file__).parent.parent.parent / "obc-firmware" / "tools" / "cmd_sys" / "cmd_sys.json",
    Path(__file__).parent.parent.parent / "obc-firmware" / "tools" / "cmd_sys" / "cmd_sys_test.json",
]

class SanAntonio(QtWidgets.QMainWindow, san_antonio.Ui_MainWindow):
    obc_assistant = None
    history_assistant = None
    command_assistant = None
    config_assistant = None
    # send and get data from thread listening in to serial
    send_q = None
    listening_thread = None
    ports = []
    past_commands = []
    past_command_index = 0

    def __init__(self, parent=None):
        super(SanAntonio, self).__init__(parent)  # I have no father
        self.setupUi(self)

        # Make it Pretty
        self.setWindowTitle("San Antonio")
        self.setWindowIcon(QtGui.QIcon('Satellite.png'))
        self.textEdit.verticalScrollBar().setValue(self.textEdit.verticalScrollBar().maximum())
        self.textEdit.setTextColor(QColor(0,0,0))

        # Get assistants
        self.obc_assistant = OBCQT(OBC(*CMDS_PATHS))
        self.history_assistant = history.HistoryHandler()
        self.config_assistant = config.ConfigHandler()
        self.command_assistant = history.CommandHistoryHandler(self.config_assistant.get_command_history())

        self.obc_assistant.logs.connect(self.handle_log)

        # Update connection state
        self.update_connection_state()

        # Populate the serial ports
        self.ports = get_serial_ports()
        self.comboBox.addItems(self.ports)

        # Connect functions to the buttons
        self.connect_button.clicked.connect(self.toggle_connection)
        self.send_button.clicked.connect(self.send_text)
        self.refresh_button.clicked.connect(self.refresh_ports)
        self.set_datetime_button.clicked.connect(self.set_current_datetime)
        self.reset_button.clicked.connect(self.reset_obc)
        self.ping_button.clicked.connect(self.ping)

        # Connect settings
        self.actionMessage_Level.triggered.connect(self.toggle_msg_lvl)
        self.actionFunction_ID.triggered.connect(self.toggle_func_id)
        self.actionTimestamps.triggered.connect(self.toggle_timestamp)

        # Shortcuts
        self.last_msg_shortbut_next = QShortcut(QtGui.QKeySequence.MoveToPreviousLine, self)
        self.last_msg_shortbut_next.activated.connect(self.get_last_command_next)
        self.last_msg_shortbut_prev = QShortcut(QtGui.QKeySequence.MoveToNextLine, self)
        self.last_msg_shortbut_prev.activated.connect(self.get_last_command_prev)

    def toggle_msg_lvl(self) -> None:
        if globs.toggle_msg_lvl_status():
            self.actionTimestamps.setText("Timestamps: ON")
        else:
            self.actionTimestamps.setText("Timestamps: OFF")

    def toggle_func_id(self) -> None:
        globs.toggle_func_id_status()

    def toggle_timestamp(self) -> None:
        globs.toggle_timestamp_status()

    def refresh_ports(self) -> None:
        # Who needs efficiency, LC is for posers.
        new_ports = get_serial_ports()
        for x in range(0, len(self.ports)):
            self.comboBox.removeItem(x)
            # TODO is this also bugged but w/e
        new_ports = list(dict.fromkeys(new_ports))
        self.comboBox.addItems(new_ports)
        self.ports = new_ports

    def closeEvent(self, a0: QtGui.QCloseEvent) -> None:
        self.obc_assistant.stop()
        return

    def keyPressEvent(self, a0: QtGui.QKeyEvent) -> None:
        if (a0.key() == QtCore.Qt.Key_Return) or (a0.key() == QtCore.Qt.Key_Enter):
            self.send_text()

    def update_connection_state(self):
        if self.obc_assistant.connected:
            self.status_label.setText(constants.serial_status_connected_text)
            self.status_label.setStyleSheet(constants.get_colour(constants.green))
            self.connect_button.setText(constants.connect_button_disconnect_text)
            self.reset_button.setDisabled(False)
            self.set_datetime_button.setDisabled(False)
            self.ping_button.setDisabled(False)
        else:
            self.status_label.setText(constants.serial_status_disconnected_text)
            self.status_label.setStyleSheet(constants.get_colour(constants.red))
            self.connect_button.setText(constants.connect_button_connect_text)
            self.reset_button.setDisabled(True)
            self.set_datetime_button.setDisabled(True)
            self.ping_button.setDisabled(True)

    def toggle_connection(self):
        if self.obc_assistant.connected:
            self.obc_assistant.stop()
        else:
            self.obc_assistant.start(str(self.comboBox.currentText()))
        self.update_connection_state()

    def add_line_to_text(self, line):
        self.history_assistant.add_line(line)
        self.textEdit.setText(self.history_assistant.history_print)
        self.textEdit.verticalScrollBar().setValue(self.textEdit.verticalScrollBar().maximum())

    def add_line_to_text_from_serial(self, line):
        self.history_assistant.add_line(str(line), "--> ")
        self.textEdit.setText(self.history_assistant.history_print)
        self.textEdit.verticalScrollBar().setValue(self.textEdit.verticalScrollBar().maximum())

    def send_text(self):
        input_text = self.input_line.text()
        if not input_text:
            return

        self.command_assistant.save_input(input_text)
        self.config_assistant.update(self.command_assistant.history)
        self.config_assistant.save()

        self.add_line_to_text(input_text)
        self.input_line.setText("")

        input_split = input_text.split()
        if len(input_split) > 0 and input_split[0].lower() == "help":
            # Special handling for help
            if len(input_split) > 1:
                # Command-specific help
                cmd_name = input_split[1]
                try:
                    cmd_sys_spec = self.obc_assistant._obc.specs.get(name=cmd_name)
                    self.add_line_to_text_from_serial(str(cmd_sys_spec))
                except OBCCmdNotFoundError as e:
                    self.add_line_to_text_from_serial(str(e))
            else:
                # General help
                self.add_line_to_text_from_serial(f"Available Commands:\n{str(self.obc_assistant._obc.specs)}")
        else:
            self.obc_assistant.execute(OBCQTRequest(
                lambda obc, cmd_str=input_text: obc.send_cmd_str(cmd_str),
                self.handle_cmd_resp
            ))

    def get_last_command_next(self):
        self.input_line.setText(self.command_assistant.get_next_command())

    def get_last_command_prev(self):
        self.input_line.setText(self.command_assistant.get_previous_command())

    def ping(self):
        self.obc_assistant.execute(OBCQTRequest(
            lambda obc: obc.ping(),
            self.handle_cmd_resp
        ))

    def set_current_datetime(self):
        self.obc_assistant.execute(OBCQTRequest(
            lambda obc: obc.set_time(),
            self.handle_cmd_resp
        ))

    def reset_obc(self):
        self.obc_assistant.execute(OBCQTRequest(
            lambda obc: obc.reset(),
            self.handle_cmd_resp
        ))

    def handle_cmd_resp(self, resp: Union[OBCResponse, OBCError]):
        if resp is not None:
            print(resp)
            self.add_line_to_text_from_serial(str(resp))

    def handle_log(self, log: OBCLog):
        line = ""

        if globs.msg_lvl_status == constants.MSG_LVL_ON:
            line += f"[{log.level.name}] "
        if globs.timestamp_status == constants.TIMESTAMPS_ON:
            line += f"[{str(log.date_time)}] "
        if globs.func_id_status == constants.FUNC_ID_ON:
            line += f"[ID: {str(log.func_id)}] "

        line += bytes_to_hexstr(log.payload, ascii_only=True)
        self.add_line_to_text_from_serial(line)

def main():
    app = QApplication(sys.argv)
    sa = SanAntonio()
    sa.show()
    app.exec_()

if __name__ == '__main__':
    main()
