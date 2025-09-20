from typing import Any, Callable
import time

from PyQt5 import QtCore

from alea.obcfw.cmd_sys import cmd_sys_spec
from alea.obcfw.log import log_spec
from alea.obcfw.task import task_spec

from alea.ttc import ttc as alea_ttc
from alea.ttc import ttc_base
from alea.ttc.protocol.generic import routing
from alea.ttc.protocol.app import app_log

class TTCQTRequest:
    """A request to perform an operation over the TTC interface in a background thread.

    Attributes:
        func (readonly): The function to call to perform the operation (will be passed an instance of a TTC).
                         The function will be invoked in a background thread.
        callback (readonly): A callback that will be called and passed the return value from calling `func`.
                             The callback will be called from the thread that owns the `TTCQT` instance.
                             Defaults to None.
    """

    def __init__(self, func: Callable[[alea_ttc.TTC], Any], callback: Callable[[Any], None] = None):
        self._func = func
        self._callback = callback

    @property
    def func(self) -> Callable[[alea_ttc.TTC], Any]:
        return self._func

    @property
    def callback(self) -> Callable[[Any], None]:
        return self._callback

class TTCQTResponse:
    """The response to an `TTCQTRequest`.

    Attributes:
        data (readonly): The return value from calling `func`.
        callback (readonly): The same callback that was originally passed in the `TTCQTRequest`.
    """

    def __init__(self, data: Any, callback: Callable[[Any], None]):
        self._data = data
        self._callback = callback

    @property
    def data(self) -> Any:
        return self._data

    @property
    def callback(self) -> Callable[[Any], None]:
        return self._callback

class TTCWorker(QtCore.QObject):
    """A base class for a worker that interacts with the TTC and can be moved to a background thread.

    There are two modes of operation for this object:

    CONTINUOUS:
        - The thread that owns this object should have its `started` signal connected to this object's `work` function so
          it can run a continuous process in the thread.
        - The `loop` function can send data out on this object's `data` signal.
        - To stop the worker, `requestInterruption()` must be called on the thread that owns the worker.

    REQUEST / RESPONSE:
        - The thread that owns this object should let its default event loop run continuously (do not use the `work` function).
        - Requests should be sent to this object's `handle_request` slot
        - Responses will be emitted on this object's `response` signal.
        - To stop the worker, a signal must be sent to this object's `handle_quit` slot with the value `True`.
    """

    data     = QtCore.pyqtSignal(object)
    response = QtCore.pyqtSignal(object)
    error    = QtCore.pyqtSignal(Exception)
    finished = QtCore.pyqtSignal()

    def __init__(self, ttc: alea_ttc.TTC, **kwargs):
        super().__init__(**kwargs)

        self._ttc = ttc

    @property
    def cancelled(self) -> bool:
        """True if a request to interrupt the thread that owns this worker has been received, otherwise False.
        """
        return QtCore.QThread.currentThread().isInterruptionRequested()

    @QtCore.pyqtSlot()
    def work(self):
        self.setup(self._ttc)
        while not self.cancelled:
            if not self.loop(self._ttc):
                break
        self.teardown(self._ttc)

        self.finished.emit()

        QtCore.QThread.currentThread().quit()

    def setup(self, ttc: alea_ttc.TTC):
        """Called once when the thread that own this worker is started,
        before `loop` begins executing.
        """
        pass

    def loop(self, ttc: alea_ttc.TTC) -> bool:
        """Called repeatedly in an infinite loop in the thread that owns this worker.
        This method must return periodically to allow the thread to be terminated cleanly.
        """
        time.sleep(0.1)
        return True

    def teardown(self, ttc: alea_ttc.TTC):
        """Called once after the final `loop` execution once the thread that owns this worker
        has been requested to terminate.
        """
        pass

    @QtCore.pyqtSlot(bool)
    def handle_quit(self, quit: bool):
        """QT slot to handle a request to terminate the thread that owns this worker.

        Args:
            quit: True if the thread should terminate. If False no action will be taken.
        """
        if quit:
            self.finished.emit()
            QtCore.QThread.currentThread().quit()

    @QtCore.pyqtSlot(TTCQTRequest)
    def handle_request(self, request: TTCQTRequest):
        """QT slot to handle an incoming request to run an operation in the thread that owns this worker.

        The request's `func` will be invoked in the worker's thread. The request's `callback`
        will be sent back to the calling thread through the `response` signal.

        Args:
            request: The request containing the operation to run and a callback.
        """
        try:
            resp = request.func(self._ttc)
        except Exception as e:
            resp = e
            self.error.emit(e)

        self.response.emit(TTCQTResponse(resp, request.callback))

class TTCQTThread(QtCore.QObject):
    """Helper class to facilitate running operations in a background thread in QT.

    There are two modes of operation for this object. See `TTCWorker` for details relating to the worker.

    CONTINUOUS:
        - The worker's `data` signal will be connected to this object's `data` signal to forward
          emitted objects.

    REQUEST / RESPONSE:
        - This object's `request` signal will be connected to the worker's `handle_request` slot to submit requests.
        - The responses will be handled internally in this class (the callback will be executed on the thread that owns
          this object).
    """

    data = QtCore.pyqtSignal(object)
    request = QtCore.pyqtSignal(TTCQTRequest)
    error = QtCore.pyqtSignal(Exception)
    quit = QtCore.pyqtSignal(bool)

    def __init__(self, name: str, ttc: alea_ttc.TTC, worker_cls: type[TTCWorker], run_continuous: bool = True, **kwargs):
        """Initializes a new `TTCQTThread` (but does not start the thread).

        NOTE: This class itself is not actually a thread, but it instantiates and manages a thread.

        Args:
            ttc: A reference to an `TTC` instance.
            worker_cls: The class for the worker object that will be instantiated and moved to a background thread.
            run_continuous: If True, this object and the worker will operate in CONTINUOUS mode (see class docs).
                            If False, this object and the worker will operate in REQUEST / RESPONSE mode (see class docs).
                            Defaults to True.
        """
        super().__init__(**kwargs)
        self._name = name
        self._ttc = ttc
        self._worker_cls = worker_cls
        self._run_continuous = run_continuous

        self._worker: TTCWorker = None
        self._thread: QtCore.QThread = None
        self._started = False

    @property
    def started(self) -> bool:
        """True if the thread has been started, otherwise False.
        """
        return self._started

    @QtCore.pyqtSlot(TTCQTResponse)
    def handle_resp(self, resp: TTCQTResponse):
        """QT slot to handle a response from the worker.
        The response `callback` will be invoked in the thread that owns this TTCQTThread object.
        """
        if resp.callback is not None:
            resp.callback(resp.data)

    def start(self):
        """Starts the thread and connect up signals/slots if the thread has not already been started.
        """
        if not self.started:
            print(f"Starting {self._name} QT thread")
            self._thread = QtCore.QThread()
            self._worker = self._worker_cls(self._ttc)

            self._worker.moveToThread(self._thread)

            # Worker signals
            if self._run_continuous:
                self._worker.data.connect(self.data)
                self._thread.started.connect(self._worker.work)
            else:
                self._worker.response.connect(self.handle_resp)
                self._worker.error.connect(self.error)
                self.request.connect(self._worker.handle_request)
                self.quit.connect(self._worker.handle_quit)

            self._thread.finished.connect(self._worker.deleteLater)

            self._thread.start()
            self._started = True

    def stop(self, timeout_ms: int = 2000):
        """Stop the thread if it is running.

        If the thread does not stop after timeout_ms milliseconds, it will be forcefully terminated.
        """
        if self.started:
            print(f"Stopping {self._name} QT thread")
            if self._run_continuous:
                self._thread.requestInterruption()
            else:
                self.quit.emit(True)

            if not self._thread.wait(timeout_ms):
                # This is a brute force termination by the OS and should only be used
                # as a last resort
                print(f"Force terminating {self._name} QT thread")
                self._thread.terminate()

            self._started = False
            self._worker = None
            self._thread = None

class OBCLogWorker(TTCWorker):
    """Implementation of `TTCWorker` that operates in CONTINUOUS mode listening to OBC logs
    and emitting them on the `data` signal.
    """

    def __init__(self, ttc: alea_ttc.TTC, **kwargs):
        super().__init__(ttc, **kwargs)

        self._logs: routing.PacketSource[app_log.OBCLog] = None

    def setup(self, ttc: alea_ttc.TTC):
        self._logs = ttc.add_log_listener(queue_size=100)

    def loop(self, ttc: alea_ttc.TTC) -> bool:
        if self._logs is None:
            return False

        recvd_logs = self._logs.read(timeout=0.1)
        if recvd_logs:
            for recvd_log in recvd_logs:
                self.data.emit(recvd_log)
        return True

    def teardown(self, ttc: alea_ttc.TTC):
        ttc.remove_log_listener(self._logs)

class TTCQT(QtCore.QObject):
    """Wrapper object for managing an `TTC` object in a QT environment.
    """

    error = QtCore.pyqtSignal(Exception)

    def __init__(self, ttc: alea_ttc.TTC, **kwargs):
        super().__init__(**kwargs)

        self._ttc = ttc

        self._log_thread = TTCQTThread("Log", self._ttc, OBCLogWorker, run_continuous=True)
        self._request_thread = TTCQTThread("Request", self._ttc, TTCWorker, run_continuous=False)

        self._request_thread.error.connect(self.error)

    @property
    def interface_type(self) -> alea_ttc.TTC.InterfaceType:
        return self._ttc.interface_type

    @property
    def cmd_sys_specs(self) -> cmd_sys_spec.OBCCmdSysSpecs:
        return self._ttc.cmd_sys_specs

    @property
    def log_specs(self) -> log_spec.OBCLogGroupSpecs:
        return self._ttc.log_specs

    @property
    def task_specs(self) -> task_spec.OBCTaskSpecs:
        return self._ttc.task_specs

    @property
    def connected(self) -> bool:
        return self._ttc.connected

    @property
    def logs(self) -> QtCore.pyqtBoundSignal:
        """QT signal that sends `OBCLog` objects as they are received.
        """
        return self._log_thread.data

    def add_obc_event_listener(self, listener: ttc_base.OBCEventListener):
        """Wrapper for TTCBase.add_obc_event_listener(...)
        """
        self._ttc.add_obc_event_listener(listener)

    def remove_obc_event_listener(self, listener: ttc_base.OBCEventListener):
        """Wrapper for TTCBase.remove_event_listener(...)
        """
        self._ttc.remove_obc_event_listener(listener)

    def clear_event_listeners(self):
        self._ttc.clear_event_listeners()

    def execute(self, request: TTCQTRequest):
        """Run an operation using the TTC interface in a background thread.
        """
        self._request_thread.request.emit(request)

    def start(self, *args, **kwargs) -> bool:
        """Open the TTC connection.

        This may spawn background threads to manage communication through the TTC interface.

        Returns:
            True if the connection was opened successfully, otherwise False.
        """
        if self._ttc.connected or self._log_thread.started:
            # Must be stopped first
            return False

        # Start the TTC
        if not self._ttc.start(*args, **kwargs):
            return False

        # Start threads
        self._log_thread.start()
        self._request_thread.start()

        return True

    def stop(self):
        """Closes a connection to the OBC and terminates the background threads.
        """
        self._log_thread.stop()
        self._request_thread.stop()
        self._ttc.stop()

class OBCTTCInterfaceProvider:
    @property
    def obc_ttc(self) -> TTCQT:
        raise NotImplementedError()

    @property
    def obc_logs(self) -> QtCore.pyqtBoundSignal:
        raise NotImplementedError()

    @property
    def obc_conn_state_changed(self) -> QtCore.pyqtBoundSignal:
        raise NotImplementedError()

    @property
    def obc_interface_changed(self) -> QtCore.pyqtBoundSignal:
        raise NotImplementedError()
