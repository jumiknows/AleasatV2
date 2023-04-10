import pathlib

from PyQt5 import QtWidgets, QtCore, QtGui

from sanantonio.backend import obcqt
from sanantonio.ui import image_panel_ui
from sanantonio.utils import console as console_utils
from sanantonio.utils import image as image_utils

class ImagePanel(QtWidgets.QWidget, image_panel_ui.Ui_ImagePanel):
    IMAGE_SCALE_PCNT = 0.95

    updated = QtCore.pyqtSignal()

    def __init__(self, obc: obcqt.OBCQT, parent=None):
        super().__init__(parent)

        self._obc = obc

        self._image: image_utils.CapturedImage = None
        self._last_save_dir: pathlib.Path = pathlib.Path("./").resolve()

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.image_label: QtWidgets.QLabel
        self.clear_btn: QtWidgets.QPushButton
        self.save_btn: QtWidgets.QPushButton

        self.setupUi(self)

        self._img_pixmap: QtGui.QPixmap = None

        self.image_label.installEventFilter(self)

        # Connect signals / slots
        self.clear_btn.clicked.connect(self.handle_clear_image)
        self.save_btn.clicked.connect(self.handle_save_image)

        self._update_img()

    def eventFilter(self, widget: QtCore.QObject, event: QtCore.QEvent):
        if (event.type() == QtCore.QEvent.Type.Resize) and (widget is self.image_label):
            self._update_img()
            return True
        return QtWidgets.QMainWindow.eventFilter(self, widget, event)

    @QtCore.pyqtSlot(image_utils.CapturedImage)
    def handle_image(self, image: image_utils.CapturedImage):
        self._image = image

        # Load image data
        self._img_pixmap = QtGui.QPixmap()
        self._img_pixmap.loadFromData(self._image.image_data, 'jpeg')

        self._update_img()

        self.show()

    @QtCore.pyqtSlot()
    def handle_clear_image(self):
        self._clear_image()

    @QtCore.pyqtSlot()
    def handle_save_image(self):
        if self._image is None:
            console_utils.print_err("No image to save")

        try:
            default_file_path = self._last_save_dir / f"ALEASAT_capture_{self._image.timestamp.strftime('%Y-%m-%d_%H%M%S')}.jpg"
            file_path, _ = QtWidgets.QFileDialog.getSaveFileName(self,
                caption="Save Image",
                directory=str(default_file_path),
                filter="All Files (*);;JPEG Images (*.jpg)",
                initialFilter="JPEG Images (*.jpg)")

            if file_path:
                file_path = pathlib.Path(file_path)
                self._last_save_dir = file_path.parent

                with open(file_path, "wb") as f:
                    f.write(self._image.image_data)

                print(f"Saved image to: {str(file_path)}")
        except Exception as e:
            console_utils.print_err(str(e))

    def _update_img(self):
        if self._img_pixmap is None:
            self._clear_image()
        else:
            self.image_label.setPixmap(self._img_pixmap.scaled(
                # If the image is scaled to the full width/height of the image_label it will trigger
                # a small increase in the image_label size which will trigger another resize event and
                # cause a recursive series of calls to this function until the size maxes out.
                #
                # To avoid this, we scale the image to only IMAGE_SCALE_PCNT of the full image_label
                # dimensions which prevents the image_label from resizing afterwards and triggering
                # the recursion.
                int(self.image_label.width() * self.IMAGE_SCALE_PCNT), int(self.image_label.height() * self.IMAGE_SCALE_PCNT),
                QtCore.Qt.AspectRatioMode.KeepAspectRatio)
            )
            self.updated.emit()

    def _clear_image(self):
        self.image_label.clear()
        self.image_label.setText("No image")
        self._img_pixmap = None
