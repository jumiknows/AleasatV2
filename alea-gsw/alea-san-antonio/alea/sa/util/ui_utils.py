from enum import Enum

from PyQt5 import QtGui

class Color(Enum):
    TRANSPARENT = "#00000000"
    RED         = "red"
    LIGHT_RED   = "#F04848"
    ORANGE      = "orange"
    DARK_ORANGE = "#DB7A18"
    BLUE        = "blue"
    GREEN       = "green"
    BLACK       = "black"
    YELLOW      = "yellow"
    WHITE       = "white"

    def as_stylesheet(self, label: str = "color") -> str:
        return f"{label}: {self.value}"

    def as_qcolor(self) -> QtGui.QColor:
        return QtGui.QColor(self.value)
