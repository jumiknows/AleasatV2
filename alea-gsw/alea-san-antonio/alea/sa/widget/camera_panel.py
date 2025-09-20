import datetime
import collections

from PyQt5 import QtWidgets, QtCore

from alea.ttc import cmd_sys
from alea.ttc.util import data_utils
from alea.ttc.protocol.app import app_log

from alea.sa.backend import ttcqt
from alea.sa.ui.control_panels import camera_panel_ui
from alea.sa.util import console, image_utils, ttc_utils

class CameraPanel(QtWidgets.QWidget, camera_panel_ui.Ui_CameraPanel):
    captured_image = QtCore.pyqtSignal(image_utils.CapturedImage)
    image_transfer_progress = QtCore.pyqtSignal(data_utils.DataProgress)

    def __init__(self, obc_provider: ttcqt.OBCTTCInterfaceProvider, parent=None):
        super().__init__(parent)

        self._obc_provider = obc_provider

        # Declare UI members with type hints - these are assigned allocated in setupUI()

        self.init_btn: QtWidgets.QPushButton
        self.capture_btn: QtWidgets.QPushButton
        self.progress_bar: QtWidgets.QProgressBar
        self.log_text: QtWidgets.QTextEdit
        self.clear_log_btn: QtWidgets.QPushButton

        # Sensor Registers
        self.sreg_addr_edit: QtWidgets.QLineEdit
        self.sreg_data_edit: QtWidgets.QLineEdit
        self.sreg_rd_btn: QtWidgets.QPushButton
        self.sreg_wr_btn: QtWidgets.QPushButton

        # Resolution
        self.res_win_x_edit: QtWidgets.QLineEdit
        self.res_win_y_edit: QtWidgets.QLineEdit
        self.res_win_width_edit: QtWidgets.QLineEdit
        self.res_win_height_edit: QtWidgets.QLineEdit
        self.res_out_width_edit: QtWidgets.QLineEdit
        self.res_out_height_edit: QtWidgets.QLineEdit
        self.res_tot_width_edit: QtWidgets.QLineEdit
        self.res_tot_height_edit: QtWidgets.QLineEdit
        self.res_set_btn: QtWidgets.QPushButton

        # White Balance
        self.wb_auto_check: QtWidgets.QCheckBox
        self.wb_top_lim_edit: QtWidgets.QLineEdit
        self.wb_bot_lim_edit: QtWidgets.QLineEdit
        self.wb_gain_r_edit: QtWidgets.QLineEdit
        self.wb_gain_g_edit: QtWidgets.QLineEdit
        self.wb_gain_b_edit: QtWidgets.QLineEdit
        self.wb_set_btn: QtWidgets.QPushButton

        # Special Digital Effects
        self.sde_check: QtWidgets.QCheckBox

        self.sde_neg_y_check: QtWidgets.QCheckBox
        self.sde_gray_check: QtWidgets.QCheckBox
        self.sde_solarize_check: QtWidgets.QCheckBox

        self.sde_fixed_y_check: QtWidgets.QCheckBox
        self.sde_fixed_y_edit: QtWidgets.QLineEdit
        self.sde_fixed_u_check: QtWidgets.QCheckBox
        self.sde_fixed_u_edit: QtWidgets.QLineEdit
        self.sde_fixed_v_check: QtWidgets.QCheckBox
        self.sde_fixed_v_edit: QtWidgets.QLineEdit

        self.sde_sat_check: QtWidgets.QCheckBox
        self.sde_sat_u_edit: QtWidgets.QLineEdit
        self.sde_sat_v_edit: QtWidgets.QLineEdit

        self.sde_hue_check: QtWidgets.QCheckBox
        self.sde_hue_cos_edit: QtWidgets.QLineEdit
        self.sde_hue_sin_edit: QtWidgets.QLineEdit

        self.sde_contrast_check: QtWidgets.QCheckBox
        self.sde_contrast_y_offset_edit: QtWidgets.QLineEdit
        self.sde_contrast_y_gain_edit: QtWidgets.QLineEdit
        self.sde_contrast_y_bright_edit: QtWidgets.QLineEdit

        self.sde_set_btn: QtWidgets.QPushButton

        self.setupUi(self)

        # Connect signals / slots
        self.init_btn.clicked.connect(self.handle_init)
        self.capture_btn.clicked.connect(self.handle_capture)

        self.sreg_rd_btn.clicked.connect(self.handle_sreg_read)
        self.sreg_wr_btn.clicked.connect(self.handle_sreg_write)

        self.res_set_btn.clicked.connect(self.handle_res_set)
        self.wb_set_btn.clicked.connect(self.handle_wb_set)
        self.sde_set_btn.clicked.connect(self.handle_sde_set)

        self.image_transfer_progress.connect(self.handle_image_transfer_progress)

        self._obc_provider.obc_logs.connect(self.handle_log)
        self.clear_log_btn.clicked.connect(self.handle_clear_log)

        # Configure UI
        self.configure_progress_bar(initializing=False, capturing=False)

        self._arduchip_logs = collections.deque(maxlen=256)

    @property
    def obc(self) -> ttcqt.TTCQT:
        return self._obc_provider.obc_ttc

    def configure_progress_bar(self, initializing: bool, capturing: bool):
        self.progress_bar.setVisible(initializing or capturing)

        if initializing:
            self.progress_bar.setRange(0, 0)
        if capturing:
            self.progress_bar.setRange(0, 100)

        self.progress_bar.setValue(0)

    @QtCore.pyqtSlot()
    def handle_init(self):
        self.configure_progress_bar(initializing=True, capturing=False)
        self.obc.execute(ttcqt.TTCQTRequest(
            lambda obc: obc.camera_init(),
            self._init_callback
        ))

    def _init_callback(self, resp: cmd_sys.OBCResponse | Exception):
        self.configure_progress_bar(initializing=False, capturing=False)

    @QtCore.pyqtSlot()
    def handle_capture(self):
        self.configure_progress_bar(initializing=False, capturing=True)

        progress_callback = lambda progress: self.image_transfer_progress.emit(progress)
        self.obc.execute(ttcqt.TTCQTRequest(
            lambda obc: obc.camera_capture(progress_callback=progress_callback),
            self._capture_callback
        ))

    @QtCore.pyqtSlot(data_utils.DataProgress)
    def handle_image_transfer_progress(self, progress: data_utils.DataProgress):
        self.progress_bar.setValue(int(progress.percent))

    def _capture_callback(self, resp: cmd_sys.OBCResponse | Exception):
        self.configure_progress_bar(initializing=False, capturing=False)

        if not ttc_utils.check_cmd_sys_resp(resp, "image_size", "image_data"):
            return

        if resp.data["image_size"] <= 0:
            console.print_err(f"Invalid image size: {resp.data['image_size']}")
            return

        self.captured_image.emit(image_utils.CapturedImage(
            resp.data["image_data"],
            datetime.datetime.now()
        ))

    @QtCore.pyqtSlot()
    def handle_sreg_read(self):
        try:
            addr = int(self.sreg_addr_edit.text(), 16)

            self.obc.execute(ttcqt.TTCQTRequest(
                lambda obc: obc.camera_read_sensor_reg(addr),
                self._sreg_read_callback
            ))
        except ValueError:
            console.print_err("Error parsing addr field")

    def _sreg_read_callback(self, resp: cmd_sys.OBCResponse | Exception):
        if not ttc_utils.check_cmd_sys_resp(resp, "ov5642_err", "data"):
            return

        if resp.data["ov5642_err"] == 0:
            self.sreg_data_edit.setText(f"{resp.data['data']:x}")

    @QtCore.pyqtSlot()
    def handle_sreg_write(self):
        try:
            addr = int(self.sreg_addr_edit.text(), 16)
            data = int(self.sreg_data_edit.text(), 16)

            self.obc.execute(ttcqt.TTCQTRequest(
                lambda obc: obc.camera_write_sensor_reg(addr, data)
            ))
        except ValueError:
            console.print_err("Error parsing camera sensor addr/data fields")

    @QtCore.pyqtSlot()
    def handle_res_set(self):
        try:
            win_x      = int(self.res_win_x_edit.text(), 0)
            win_y      = int(self.res_win_y_edit.text(), 0)
            win_width  = int(self.res_win_width_edit.text(), 0)
            win_height = int(self.res_win_height_edit.text(), 0)
            out_width  = int(self.res_out_width_edit.text(), 0)
            out_height = int(self.res_out_height_edit.text(), 0)
            tot_width  = int(self.res_tot_width_edit.text(), 0)
            tot_height = int(self.res_tot_height_edit.text(), 0)

            self.obc.execute(ttcqt.TTCQTRequest(
                lambda obc: obc.camera_set_resolution(win_x, win_y, win_width, win_height, out_width, out_height, tot_width, tot_height)
            ))
        except ValueError:
            console.print_err("Error parsing camera resolution fields")

    @QtCore.pyqtSlot()
    def handle_wb_set(self):
        try:
            is_auto = self.wb_auto_check.isChecked()
            if is_auto:
                top_limit    = int(self.wb_top_lim_edit.text(), 0)
                bottom_limit = int(self.wb_bot_lim_edit.text(), 0)

                self.obc.execute(ttcqt.TTCQTRequest(
                    lambda obc: obc.camera_set_white_balance_auto_simple(top_limit, bottom_limit)
                ))
            else:
                gain_r = int(self.wb_gain_r_edit.text(), 0)
                gain_g = int(self.wb_gain_g_edit.text(), 0)
                gain_b = int(self.wb_gain_b_edit.text(), 0)

                self.obc.execute(ttcqt.TTCQTRequest(
                    lambda obc: obc.camera_set_white_balance_manual(gain_r, gain_g, gain_b)
                ))
        except ValueError:
            console.print_err("Error parsing camera white balance fields")

    @QtCore.pyqtSlot()
    def handle_sde_set(self):
        try:
            sde_enable = self.sde_check.isChecked()
            fixed_y = (int(self.sde_fixed_y_edit.text(), 0)) if self.sde_fixed_y_check.isChecked() else None
            fixed_u = (int(self.sde_fixed_u_edit.text(), 0)) if self.sde_fixed_u_check.isChecked() else None
            fixed_v = (int(self.sde_fixed_v_edit.text(), 0)) if self.sde_fixed_v_check.isChecked() else None
            neg_y = self.sde_neg_y_check.isChecked()
            gray = self.sde_gray_check.isChecked()
            solarize = self.sde_solarize_check.isChecked()
            contrast = None
            if self.sde_contrast_check.isChecked():
                contrast = (
                    int(self.sde_contrast_y_offset_edit.text(), 0),
                    int(self.sde_contrast_y_gain_edit.text(), 0),
                    int(self.sde_contrast_y_bright_edit.text(), 0),
                )
            saturation = None
            if self.sde_sat_check.isChecked():
                saturation = (
                    int(self.sde_sat_u_edit.text(), 0),
                    int(self.sde_sat_v_edit.text(), 0),
                )
            hue = None
            if self.sde_hue_check.isChecked():
                hue = (
                    int(self.sde_hue_cos_edit.text(), 0),
                    int(self.sde_hue_sin_edit.text(), 0),
                )

            self.obc.execute(ttcqt.TTCQTRequest(
                lambda obc: obc.camera_set_special_digital_effects(
                                    enable     = sde_enable,
                                    fixed_y    = fixed_y,
                                    neg_y      = neg_y,
                                    gray       = gray,
                                    fixed_v    = fixed_v,
                                    fixed_u    = fixed_u,
                                    contrast   = obc.CameraContrast(*contrast) if contrast else None,
                                    saturation = obc.CameraSaturation(*saturation) if saturation else None,
                                    hue        = obc.CameraHue(*hue) if hue else None,
                                    solarize   = solarize,
                            )
            ))
        except ValueError as e:
            console.print_err(f"Error parsing camera special digital effects fields: {e}")

    @QtCore.pyqtSlot(app_log.OBCLog)
    def handle_log(self, log: app_log.OBCLog):
        if log.group_name == "LOG_ARDUCHIP":
            if (log.signal_name == "READ_REG") or (log.signal_name == "WRITE_REG"):
                text = f"[ {log.signal_name:<20} ] MIBSPI ERR = {log.data['mibspi_err']:4d} | ADDR = 0x{log.data['addr']:02x} | DATA = 0x{log.data['data']:02x} ({'R' if log.signal_name == 'READ_REG' else 'W'})"
            else:
                text = f"{log.signal_name:-^30}"
            self._arduchip_logs.append(text)
            self._update_log()

    @QtCore.pyqtSlot()
    def handle_clear_log(self):
        self._arduchip_logs.clear()
        self._update_log()

    def _update_log(self):
        text = "\n".join(self._arduchip_logs)
        self.log_text.setText(text)
        self.log_text.verticalScrollBar().setValue(self.log_text.verticalScrollBar().maximum())
