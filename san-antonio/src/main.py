import sys
import datetime
import globs

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtGui import QColor
from PyQt5.QtWidgets import QApplication, QShortcut

import obcpy.utils
from obcpy.obc_message import OBCMessage

import san_antonio, history, config, constants
from qt_obc import OBCQT

class SanAntonio(QtWidgets.QMainWindow, san_antonio.Ui_MainWindow):
    obc_assistant = None
    history_assistant = None
    commmand_assistant = None
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
        self.obc_assistant = OBCQT()
        self.history_assistant = history.HistoryHandler()
        self.config_assistant = config.ConfigHandler()
        self.commmand_assistant = history.CommandHistoryHandler(self.config_assistant.get_command_history())

        self.obc_assistant.msg_received.connect(self.handle_msg)

        # Update connection state
        self.update_connection_state()

        # Populate the serial ports
        self.ports = obcpy.utils.get_serial_ports()
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
        new_ports = obcpy.utils.get_serial_ports()
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
        if self.obc_assistant.obc.connected:
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
        if self.obc_assistant.obc.connected:
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

        self.commmand_assistant.save_input(input_text)
        self.config_assistant.update(self.commmand_assistant.history)
        self.config_assistant.save()
        self.obc_assistant.obc.send_raw(input_text)
        self.add_line_to_text(input_text)
        self.input_line.setText("")

    def get_last_command_next(self):
        self.input_line.setText(self.commmand_assistant.get_next_command())

    def get_last_command_prev(self):
        self.input_line.setText(self.commmand_assistant.get_previous_command())

    def ping(self):
        self.obc_assistant.obc.ping()

    def set_current_datetime(self):
        self.obc_assistant.obc.set_current_datetime()

    def reset_obc(self):
        self.obc_assistant.obc.reset()

    def handle_msg(self, msg: OBCMessage):
        line = ""

        if globs.msg_lvl_status == constants.MSG_LVL_ON:
            line += f"[{OBCMessage.LEVEL_MAP[msg.level]}] "
        if globs.timestamp_status == constants.TIMESTAMPS_ON:
            line += f"[{msg.timestamp.strftime('%Y-%m-%d %H:%M:%S')}] "
        if globs.func_id_status == constants.FUNC_ID_ON:
            line += f"[ID: {str(msg.func_id)}] "

        line += msg.payload
        self.add_line_to_text_from_serial(line)

def main():
    app = QApplication(sys.argv)
    sa = SanAntonio()
    sa.show()
    app.exec_()

if __name__ == '__main__':
    main()
