from typing import List
import sys
import platform
import contextlib
import pathlib
import ctypes

from PyQt5 import QtGui, QtWidgets
from PyQt5.QtCore import Qt

import obcpy.obc
from obcpy.obc import obc_base

from sanantonio.backend import obcqt
from sanantonio.widget import main_window
from sanantonio.utils import console as console_utils

class SanAntonio:
    APP_ID = 'aleasat.sanantonio.0'

    def __init__(self, cmd_sys_specs_paths: List[pathlib.Path], log_specs_path: pathlib.Path):
        self._obcs = {
            obc_base.OBCBase.InterfaceType.OBC_SERIAL.id   : obcqt.OBCQT(obcpy.obc.OBC(cmd_sys_specs_paths, log_specs_path, obc_base.OBCBase.InterfaceType.OBC_SERIAL)),
            obc_base.OBCBase.InterfaceType.COMMS_SERIAL.id : obcqt.OBCQT(obcpy.obc.OBC(cmd_sys_specs_paths, log_specs_path, obc_base.OBCBase.InterfaceType.COMMS_SERIAL)),
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

        self._window = main_window.MainWindow(self._obcs)
        self._window.showMaximized()

        # Capture all stdout output and send it on the self._window.handle_stdout signal
        with console_utils.StdoutGUI(stdout_written=self._window.stdout_written) as stdoutGUI:
            with contextlib.redirect_stdout(stdoutGUI):
                print("San Antonio Started!")
                app.exec()
