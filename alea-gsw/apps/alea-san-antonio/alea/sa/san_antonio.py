import sys
import platform
import contextlib
import pathlib
import ctypes
import logging

from PyQt5 import QtGui, QtWidgets
from PyQt5.QtCore import Qt

from alea.ttc import ttc as alea_ttc
from alea.ttc import ttc_base

from alea.sa.backend import ttcqt
from alea.sa.util import console
from alea.sa.widget import main_window

class SanAntonio:
    APP_ID = 'alea.sa.0'

    def __init__(self, cmd_sys_specs_paths: list[pathlib.Path] = None, log_specs_paths: list[pathlib.Path] = None, task_specs_paths: list[pathlib.Path] = None):
        self._ttcs = {
            ttc_base.TTCBase.InterfaceType.OBC_SERIAL.id   : ttcqt.TTCQT(alea_ttc.TTC(ttc_base.TTCBase.InterfaceType.OBC_SERIAL, cmd_sys_specs_paths, log_specs_paths, task_specs_paths)),
            ttc_base.TTCBase.InterfaceType.COMMS_SERIAL.id : ttcqt.TTCQT(alea_ttc.TTC(ttc_base.TTCBase.InterfaceType.COMMS_SERIAL, cmd_sys_specs_paths, log_specs_paths, task_specs_paths)),
        }

    def run(self):
        app = QtWidgets.QApplication(sys.argv)

        if platform.system() == "Windows":
            # Make Windows use our icon for the taskbar
            # See https://stackoverflow.com/questions/1551605/how-to-set-applications-taskbar-icon-in-windows-7
            try:
                ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(SanAntonio.APP_ID)
            except:
                # Allow this to fail in case our platform test wasn't enough
                pass

        # Force the style to be the same on all OSs
        app.setStyle("Fusion")

        # Use a palette to switch to dark colors
        palette = QtGui.QPalette()
        palette.setColor(QtGui.QPalette.Window, QtGui.QColor(53, 53, 53))
        palette.setColor(QtGui.QPalette.WindowText, Qt.white)
        palette.setColor(QtGui.QPalette.Base, QtGui.QColor(25, 25, 25))
        palette.setColor(QtGui.QPalette.AlternateBase, QtGui.QColor(53, 53, 53))
        palette.setColor(QtGui.QPalette.ToolTipBase, Qt.black)
        palette.setColor(QtGui.QPalette.ToolTipText, Qt.white)
        palette.setColor(QtGui.QPalette.Text, Qt.white)
        palette.setColor(QtGui.QPalette.Button, QtGui.QColor(53, 53, 53))
        palette.setColor(QtGui.QPalette.ButtonText, Qt.white)
        palette.setColor(QtGui.QPalette.BrightText, Qt.red)
        palette.setColor(QtGui.QPalette.Link, QtGui.QColor(42, 130, 218))
        palette.setColor(QtGui.QPalette.Highlight, QtGui.QColor(42, 130, 218))
        palette.setColor(QtGui.QPalette.HighlightedText, Qt.black)
        app.setPalette(palette)

        self._window = main_window.MainWindow(self._ttcs)
        self._window.showMaximized()

        # Capture all stdout output and send it on the self._window.handle_stdout signal
        with console.StdoutGUI(stdout_written=self._window.stdout_written) as stdoutGUI:
            with contextlib.redirect_stdout(stdoutGUI):
                logging.basicConfig(level=logging.DEBUG, handlers=[logging.StreamHandler(stdoutGUI)], format="%(message)s")
                print("San Antonio Started!")
                app.exec()
