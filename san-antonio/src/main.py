from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtGui import QColor
from PyQt5.QtWidgets import QApplication, QPushButton, QLabel, QShortcut
import sys, threading, queue, time
import san_antonio, constants, serial_tools, history, config
import ports as ports_tools
import globs

class SanAntonio(QtWidgets.QMainWindow, san_antonio.Ui_MainWindow):
    serial_assistant = None
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

        # setup queues
        self.send_q = queue.Queue()

        # Get assistants
        self.serial_assistant = serial_tools.SerialHandler(self.send_q)
        self.history_assistant = history.HistoryHandler()
        self.config_assistant = config.ConfigHandler()
        self.commmand_assistant = history.CommandHistoryHandler(self.config_assistant.get_command_history())

        # Set state
        self.set_serial_state()

        # Populate the serial ports
        self.ports = ports_tools.get_serial_ports()
        self.comboBox.addItems(self.ports)

        # Connect functions to the buttons
        self.connect_button.clicked.connect(self.connect_to_serial_port)
        self.send_button.clicked.connect(self.send_text)
        self.refresh_button.clicked.connect(self.refresh_ports)

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
        new_ports = ports_tools.get_serial_ports()
        for x in range(0, len(self.ports)):
            self.comboBox.removeItem(x)
            # TODO is this also bugged but w/e
        new_ports = list(dict.fromkeys(new_ports))
        self.comboBox.addItems(new_ports)
        self.ports = new_ports

    def closeEvent(self, a0: QtGui.QCloseEvent) -> None:
        # TODO clean termination
        return

    def keyPressEvent(self, a0: QtGui.QKeyEvent) -> None:
        if a0.key() == QtCore.Qt.Key_Return:
            self.send_text()

    def set_serial_state(self):
        serial_status_text, serial_status_color = self.serial_assistant.get_status()
        self.status_label.setText(serial_status_text)
        self.status_label.setStyleSheet(serial_status_color)

    def connect_to_serial_port(self):
        self.serial_assistant.connect(str(self.comboBox.currentText()))
        self.set_serial_state()
        if self.serial_assistant.connected() and self.listening_thread is None:
            # set thread
            self.listening_thread = self.serial_assistant
            self.listening_thread.signal.connect(self.add_line_to_text_from_serial)
            self.listening_thread.start()

    def add_line_to_text(self, line):
        self.history_assistant.add_line(line)
        self.textEdit.setText(self.history_assistant.history_print)
        self.textEdit.verticalScrollBar().setValue(self.textEdit.verticalScrollBar().maximum())

    def add_line_to_text_from_serial(self, line):
        self.history_assistant.add_line(line, "--> ")
        self.textEdit.setText(self.history_assistant.history_print)
        self.textEdit.verticalScrollBar().setValue(self.textEdit.verticalScrollBar().maximum())

    def send_text(self):
        input_text = self.input_line.text()
        if not input_text:
            return

        self.commmand_assistant.save_input(input_text)
        self.config_assistant.update(self.commmand_assistant.history)
        self.config_assistant.save()
        self.send_q.put(input_text)
        self.add_line_to_text(input_text)
        self.input_line.setText("")
        
    def get_last_command_next(self):
        self.input_line.setText(self.commmand_assistant.get_next_command())

    def get_last_command_prev(self):
        self.input_line.setText(self.commmand_assistant.get_previous_command())

        

def main():
    app = QApplication(sys.argv)
    sa = SanAntonio()
    sa.show()
    app.exec_()


if __name__ == '__main__':
    main()
