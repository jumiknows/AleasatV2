from PyQt5 import QtCore

from obcpy.obc_message import OBCMessage
from obcpy.obc_handler import OBCHandler

class OBCRXWorker(QtCore.QObject):
    msg_received = QtCore.pyqtSignal(object)
    finished = QtCore.pyqtSignal()

    def __init__(self, obc_handler: OBCHandler, **kwargs):
        super().__init__(**kwargs)

        self._obc = obc_handler

    @property
    def cancelled(self) -> bool:
        return QtCore.QThread.currentThread().isInterruptionRequested()

    @QtCore.pyqtSlot()
    def work(self):
        while not self.cancelled:
            msg = self._obc.recv(timeout=0.1)
            if msg is not None:
                self.msg_received.emit(msg)

        self.finished.emit()

        QtCore.QThread.currentThread().quit()

class OBCQT(QtCore.QObject):
    msg_received = QtCore.pyqtSignal(OBCMessage)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.obc = OBCHandler()

        self._worker: OBCRXWorker = None
        self._thread: QtCore.QThread = None

    def start(self, serial_port: str) -> bool:
        if self.obc.start(serial_port):
            print("Starting OBC QT threads")
            self._worker = OBCRXWorker(self.obc)
            self._thread = QtCore.QThread()

            self._worker.moveToThread(self._thread)

            self._worker.msg_received.connect(self.msg_received)

            self._thread.started.connect(self._worker.work)
            self._thread.finished.connect(self._worker.deleteLater)

            self._thread.start()
            return True
        return False

    def stop(self):
        if (self._thread is not None) and (self._worker is not None):
            print("Stopping OBC QT threads")
            self._thread.requestInterruption()
            self._thread.wait()

            self.obc.stop()

            self._worker = None
            self._thread = None
