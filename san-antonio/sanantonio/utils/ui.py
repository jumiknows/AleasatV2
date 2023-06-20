from enum import Enum

from PyQt5 import QtGui

class Color(Enum):
    RED    = "red"
    BLUE   = "blue"
    GREEN  = "green"
    BLACK  = "black"
    YELLOW = "yellow"
    WHITE  = "white"

    def as_stylesheet(self, label: str = "color") -> str:
        return f"{label}: {self.value}"

    def as_qcolor(self) -> QtGui.QColor:
        return QtGui.QColor(self.value)
