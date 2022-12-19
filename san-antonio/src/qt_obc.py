from PyQt5 import QtCore

from obcpy.protocol.routing import PacketSource
from obcpy.obc_protocol.log import OBCLog

from obcpy.obc_handler import OBCHandler

class OBCRXWorker(QtCore.QObject):
    log_received = QtCore.pyqtSignal(OBCLog)
    finished = QtCore.pyqtSignal()

    def __init__(self, log_source: PacketSource[OBCLog], **kwargs):
        super().__init__(**kwargs)

        self._logs = log_source

    @property
    def cancelled(self) -> bool:
        return QtCore.QThread.currentThread().isInterruptionRequested()

    @QtCore.pyqtSlot()
    def work(self):
        while not self.cancelled:
            logs = self._logs.read(timeout=0.1)
            if logs:
                for log in logs:
                    print(log)
                    self.log_received.emit(log)

        self.finished.emit()

        QtCore.QThread.currentThread().quit()

class OBCQT(QtCore.QObject):
    log_received = QtCore.pyqtSignal(OBCLog)

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.obc = OBCHandler()

        self._worker: OBCRXWorker = None
        self._thread: QtCore.QThread = None

    def start(self, serial_port: str) -> bool:
        if self.obc.start(serial_port):
            print("Starting OBC QT threads")
            self._worker = OBCRXWorker(self.obc.interface.protocol.add_log_dest())
            self._thread = QtCore.QThread()

            self._worker.moveToThread(self._thread)

            self._worker.log_received.connect(self.log_received)

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
