from typing import Any
import sys

from PyQt5 import QtCore

def print_err(message: str):
    """Print a message to stdout with error styling.

    Args:
        message: The message to print
    """

    print(f"<span style='color:red'>{message}</span>")

class StdoutGUI(QtCore.QObject):
    """Replacement for stdout that emits messages on a Qt signal and prints them to the system stdout.

    Use this class a context manager wrapping a contextlib.redirect_stdout:
    ```
    with StdoutGUI(stdout_written=<Qt signal>) as stdoutGUI:
        with contextlib.redirect_stdout(stdoutGUI):
    ```
    """

    stdout_written = QtCore.pyqtSignal(str)

    def __enter__(self) -> "StdoutGUI":
        self._stdout = sys.stdout
        return self

    def __exit__(self, *exc) -> bool:
        self._stdout = None
        return False

    def isatty(self) -> bool:
        return False

    def flush(self):
        if self._stdout is not None:
            self._stdout.flush()

    def write(self, s: Any) -> int:
        self.stdout_written.emit(str(s))

        # Also write to the original stream
        self._stdout.write(s)
