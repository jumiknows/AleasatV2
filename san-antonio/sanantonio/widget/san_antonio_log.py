from typing import List

from PyQt5 import QtWidgets, QtCore

from sanantonio.backend import obcqt
from sanantonio.ui import san_antonio_log_ui

class SanAntonioLog(QtWidgets.QWidget, san_antonio_log_ui.Ui_SanAntonioLog):
    MAX_LOG_LEN = 65536
    SEP_CHAR = "\n"

    def __init__(self, obc: obcqt.OBCQT, parent=None):
        super().__init__(parent)

        self._obc = obc
        self._text = ""

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.sa_log_text: QtWidgets.QTextEdit
        self.sa_log_clear_btn: QtWidgets.QPushButton

        self.setupUi(self)

        # Connect signals / slots
        self.sa_log_clear_btn.clicked.connect(self.clear_output)

    @QtCore.pyqtSlot(str)
    def handle_stdout(self, message: str):
        # WARNING: DO NOT CALL print FROM THIS METHOD

        # To limit the memory consumed by this widget, we remove messages from the beginning
        # of the log once we reach a certain overall character count.
        # To ensure we remove entire messages rather than parts of a message, all the messages
        # are separated by the (non-rendering) SEP_CHAR string. If the message already contains
        # this string, we pro-actively split the message at these points before storing it.

        # Split the message if it contains the SEP_CHAR
        for msg in message.split(self.SEP_CHAR):
            # Remove leading / trailing whitespace
            msg = msg.strip()
            if len(msg) > 0:
                # Wrap message in paragraph tags so it renders with line breaks as HTML
                self._text += f"<p>{msg}</p>{self.SEP_CHAR}"

        # Trim from beginning of log if necessary
        text_len = len(self._text)
        offset = 0
        while (text_len - offset) > self.MAX_LOG_LEN:
            next_msg_idx = self._text.find("\n", offset)

            if next_msg_idx > 0:
                offset = next_msg_idx + 1
            else:
                break

        self._text = self._text[offset:]

        self.sa_log_text.setHtml(self._text)
        self.sa_log_text.verticalScrollBar().setValue(self.sa_log_text.verticalScrollBar().maximum())

    @QtCore.pyqtSlot()
    def clear_output(self):
        self._text = ""
        self.sa_log_text.setHtml(self._text)
