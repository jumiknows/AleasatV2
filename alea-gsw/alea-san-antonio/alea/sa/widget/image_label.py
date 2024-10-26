from PyQt5 import QtWidgets, QtGui, QtCore

class ImageLabel(QtWidgets.QLabel):
    def __init__(self, image_path: str, parent=None):
        super().__init__(parent)

        self.image = QtGui.QPixmap(image_path)

    def _update_pixmap(self):
        self.setPixmap(self.image.scaled(self.width(), self.height(), QtCore.Qt.AspectRatioMode.KeepAspectRatio, QtCore.Qt.TransformationMode.SmoothTransformation))

    def resizeEvent(self, a0: QtGui.QResizeEvent):
        super().resizeEvent(a0)

        self._update_pixmap()
