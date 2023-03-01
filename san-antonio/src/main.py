from typing import Union
import sys
import globs
from pathlib import Path
from datetime import datetime

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtGui import QColor
from PyQt5.QtWidgets import QApplication, QShortcut, QFileDialog

from obcpy.obc import OBC
from obcpy.cmd_sys.resp import OBCResponse
from obcpy.utils.exc import OBCError, OBCCmdNotFoundError
from obcpy.utils.serial import get_serial_ports
from obcpy.utils.data import bytes_to_hexstr
from obcpy.obc_protocol.log import OBCLog

import san_antonio, history, config, constants
from obcqt import OBCQT, OBCQTRequest

CMDS_PATHS = [
    Path(__file__).resolve().parent.parent.parent / "obc-firmware" / "tools" / "cmd_sys" / "cmd_sys.json",
    Path(__file__).resolve().parent.parent.parent / "obc-firmware" / "tools" / "cmd_sys" / "cmd_sys_test.json",
]

class SanAntonio(QtWidgets.QMainWindow, san_antonio.Ui_MainWindow):
    def __init__(self, parent=None):
        super(SanAntonio, self).__init__(parent)  # I have no father
        self.setupUi(self)

        # Make it Pretty
        self.setWindowTitle("San Antonio")
        self.setWindowIcon(QtGui.QIcon('Satellite.png'))
        self.textEdit.verticalScrollBar().setValue(self.textEdit.verticalScrollBar().maximum())
        self.textEdit.setTextColor(QColor(0,0,0))

        # Get assistants
        self.obc_assistant = OBCQT(OBC(*CMDS_PATHS))
        self.history_assistant = history.HistoryHandler()
        self.config_assistant = config.ConfigHandler()
        self.command_assistant = history.CommandHistoryHandler(self.config_assistant.get_command_history())

        self.obc_assistant.logs.connect(self.handle_log)

        # Camera
        self.img_data: bytes = None
        self.img_pixmap: QtGui.QPixmap = None
        self.img_timestamp: datetime = None
        self.last_save_dir: Path = Path("./").resolve()

        # Populate the serial ports
        self.ports = get_serial_ports()
        self.comboBox.addItems(self.ports)

        # Connect functions to the buttons
        self.connect_button.clicked.connect(self.toggle_connection)
        self.clear_button.clicked.connect(self.clear_text)
        self.send_button.clicked.connect(self.send_text)
        self.refresh_button.clicked.connect(self.refresh_ports)
        self.set_datetime_button.clicked.connect(self.set_current_datetime)
        self.reset_button.clicked.connect(self.reset_obc)
        self.ping_button.clicked.connect(self.ping)
        self.cam_init_button.clicked.connect(self.cam_init)
        self.cam_capture_button.clicked.connect(self.cam_capture)
        self.cam_wr_sreg_button.clicked.connect(self.cam_wr_sreg)
        self.cam_rd_sreg_button.clicked.connect(self.cam_rd_sreg)
        self.cam_res_button.clicked.connect(self.cam_res)
        self.cam_wb_button.clicked.connect(self.cam_white_bal)
        self.cam_sde_button.clicked.connect(self.cam_sde)
        self.clear_image_button.clicked.connect(self.clear_image)
        self.save_image_button.clicked.connect(self.save_image)

        self.obc_buttons = [
            self.send_button,
            self.reset_button,
            self.ping_button,
            self.set_datetime_button,
            self.cam_init_button,
            self.cam_capture_button,
            self.cam_wr_sreg_button,
            self.cam_rd_sreg_button,
            self.cam_res_button,
            self.cam_wb_button,
            self.cam_sde_button,
        ]

        # Camera Fields
        cam_sreg_field_width = 50
        self.cam_sreg_addr_field.setFixedWidth(cam_sreg_field_width)
        self.cam_sreg_data_field.setFixedWidth(cam_sreg_field_width)

        cam_res_field_width = 50
        self.cam_res_win_x_field.setFixedWidth(cam_res_field_width)
        self.cam_res_win_y_field.setFixedWidth(cam_res_field_width)
        self.cam_res_win_width_field.setFixedWidth(cam_res_field_width)
        self.cam_res_win_height_field.setFixedWidth(cam_res_field_width)
        self.cam_res_out_width_field.setFixedWidth(cam_res_field_width)
        self.cam_res_out_height_field.setFixedWidth(cam_res_field_width)
        self.cam_res_tot_width_field.setFixedWidth(cam_res_field_width)
        self.cam_res_tot_height_field.setFixedWidth(cam_res_field_width)

        cam_wb_field_width = 50
        self.cam_wb_top_lim_field.setFixedWidth(cam_wb_field_width)
        self.cam_wb_bot_lim_field.setFixedWidth(cam_wb_field_width)
        self.cam_wb_gain_r_field.setFixedWidth(cam_wb_field_width)
        self.cam_wb_gain_g_field.setFixedWidth(cam_wb_field_width)
        self.cam_wb_gain_b_field.setFixedWidth(cam_wb_field_width)

        cam_sde_field_width = 60
        self.cam_sde_fixed_y_field.setFixedWidth(cam_sde_field_width)
        self.cam_sde_fixed_u_field.setFixedWidth(cam_sde_field_width)
        self.cam_sde_fixed_v_field.setFixedWidth(cam_sde_field_width)
        self.cam_sde_contrast_y_offset_field.setFixedWidth(cam_sde_field_width)
        self.cam_sde_contrast_y_gain_field.setFixedWidth(cam_sde_field_width)
        self.cam_sde_contrast_y_bright_field.setFixedWidth(cam_sde_field_width)
        self.cam_sde_sat_u_field.setFixedWidth(cam_sde_field_width)
        self.cam_sde_sat_v_field.setFixedWidth(cam_sde_field_width)
        self.cam_sde_hue_cos_field.setFixedWidth(cam_sde_field_width)
        self.cam_sde_hue_sin_field.setFixedWidth(cam_sde_field_width)

        # Connect settings
        self.actionMessage_Level.triggered.connect(self.toggle_msg_lvl)
        self.actionFunction_ID.triggered.connect(self.toggle_func_id)
        self.actionTimestamps.triggered.connect(self.toggle_timestamp)

        # Shortcuts
        self.last_msg_shortbut_next = QShortcut(QtGui.QKeySequence.MoveToPreviousLine, self)
        self.last_msg_shortbut_next.activated.connect(self.get_last_command_next)
        self.last_msg_shortbut_prev = QShortcut(QtGui.QKeySequence.MoveToNextLine, self)
        self.last_msg_shortbut_prev.activated.connect(self.get_last_command_prev)

        # Event Listeners
        self.image_label.installEventFilter(self)

        # Update UI
        self.update_connection_state()
        self.update_img()

    def eventFilter(self, widget: QtCore.QObject, event: QtCore.QEvent):
        if (event.type() == QtCore.QEvent.Type.Resize) and (widget is self.image_label):
            self.update_img()
            return True
        return QtWidgets.QMainWindow.eventFilter(self, widget, event)


    def toggle_msg_lvl(self) -> None:
        if globs.toggle_msg_lvl_status():
            self.actionTimestamps.setText("Timestamps: ON")
        else:
            self.actionTimestamps.setText("Timestamps: OFF")

    def toggle_func_id(self) -> None:
        globs.toggle_func_id_status()

    def toggle_timestamp(self) -> None:
        globs.toggle_timestamp_status()

    def refresh_ports(self) -> None:
        # Who needs efficiency, LC is for posers.
        new_ports = get_serial_ports()
        for x in range(0, len(self.ports)):
            self.comboBox.removeItem(x)
            # TODO is this also bugged but w/e
        new_ports = list(dict.fromkeys(new_ports))
        self.comboBox.addItems(new_ports)
        self.ports = new_ports

    def closeEvent(self, a0: QtGui.QCloseEvent) -> None:
        self.obc_assistant.stop()
        return

    def keyPressEvent(self, a0: QtGui.QKeyEvent) -> None:
        if (a0.key() == QtCore.Qt.Key_Return) or (a0.key() == QtCore.Qt.Key_Enter):
            self.send_text()

    def update_connection_state(self):
        if self.obc_assistant.connected:
            self.status_label.setText(constants.serial_status_connected_text)
            self.status_label.setStyleSheet(constants.get_colour(constants.green))
            self.connect_button.setText(constants.connect_button_disconnect_text)

            for button in self.obc_buttons:
                button.setDisabled(False)
        else:
            self.status_label.setText(constants.serial_status_disconnected_text)
            self.status_label.setStyleSheet(constants.get_colour(constants.red))
            self.connect_button.setText(constants.connect_button_connect_text)

            for button in self.obc_buttons:
                button.setDisabled(True)

    def toggle_connection(self):
        if self.obc_assistant.connected:
            self.obc_assistant.stop()
        else:
            self.obc_assistant.start(str(self.comboBox.currentText()))
        self.update_connection_state()

    def add_line_to_text(self, line):
        self.history_assistant.add_line(line)
        self.textEdit.setText(self.history_assistant.history_print)
        self.textEdit.verticalScrollBar().setValue(self.textEdit.verticalScrollBar().maximum())

    def add_line_to_text_from_serial(self, line):
        self.history_assistant.add_line(str(line), "--> ")
        self.textEdit.setText(self.history_assistant.history_print)
        self.textEdit.verticalScrollBar().setValue(self.textEdit.verticalScrollBar().maximum())

    def send_text(self):
        input_text = self.input_line.text()
        if not input_text:
            return

        self.command_assistant.save_input(input_text)
        self.config_assistant.update(self.command_assistant.history)
        self.config_assistant.save()

        self.add_line_to_text(input_text)
        self.input_line.setText("")

        input_split = input_text.split()
        if len(input_split) > 0 and input_split[0].lower() == "help":
            # Special handling for help
            if len(input_split) > 1:
                # Command-specific help
                cmd_name = input_split[1]
                try:
                    cmd_sys_spec = self.obc_assistant._obc.specs.get(name=cmd_name)
                    self.add_line_to_text_from_serial(str(cmd_sys_spec))
                except OBCCmdNotFoundError as e:
                    self.add_line_to_text_from_serial(str(e))
            else:
                # General help
                self.add_line_to_text_from_serial(f"Available Commands:\n{str(self.obc_assistant._obc.specs)}")
        else:
            self.obc_assistant.execute(OBCQTRequest(
                lambda obc, cmd_str=input_text: obc.send_cmd_str(cmd_str),
                self.handle_cmd_resp
            ))

    def clear_text(self):
        self.history_assistant.clear()
        self.textEdit.setText(self.history_assistant.history_print)

    def get_last_command_next(self):
        self.input_line.setText(self.command_assistant.get_next_command())

    def get_last_command_prev(self):
        self.input_line.setText(self.command_assistant.get_previous_command())

    def ping(self):
        self.obc_assistant.execute(OBCQTRequest(
            lambda obc: obc.ping(),
            self.handle_cmd_resp
        ))

    def set_current_datetime(self):
        self.obc_assistant.execute(OBCQTRequest(
            lambda obc: obc.set_time(),
            self.handle_cmd_resp
        ))

    def reset_obc(self):
        self.obc_assistant.execute(OBCQTRequest(
            lambda obc: obc.reset(),
            self.handle_cmd_resp
        ))

    def cam_init(self):
        self.obc_assistant.execute(OBCQTRequest(
            lambda obc: obc.camera_init(),
            self.handle_cmd_resp
        ))

    def cam_capture(self):
        # def req_func(obc: OBC):
        #     resp = obc.camera_init()

        #     if resp["arducam_err"] == 0:
        #         print(resp)
        #         resp = obc.camera_capture()

        #     return resp

        # self.obc_assistant.execute(OBCQTRequest(req_func, self.handle_img))

        self.obc_assistant.execute(OBCQTRequest(
            lambda obc: obc.camera_capture(),
            self.handle_img
        ))

    def cam_wr_sreg(self):
        try:
            addr = int(self.cam_sreg_addr_field.text(), 16)
            data = int(self.cam_sreg_data_field.text(), 16)

            self.obc_assistant.execute(OBCQTRequest(
                lambda obc: obc.camera_write_sensor_reg(addr, data),
                self.handle_cmd_resp
            ))
        except ValueError:
            print("Error parsing addr/data fields")

    def cam_rd_sreg(self):
        try:
            addr = int(self.cam_sreg_addr_field.text(), 16)

            self.obc_assistant.execute(OBCQTRequest(
                lambda obc: obc.camera_read_sensor_reg(addr),
                self.handle_cmd_resp
            ))
        except ValueError:
            print("Error parsing addr field")

    def cam_res(self):
        try:
            win_x      = int(self.cam_res_win_x_field.text(), 0)
            win_y      = int(self.cam_res_win_y_field.text(), 0)
            win_width  = int(self.cam_res_win_width_field.text(), 0)
            win_height = int(self.cam_res_win_height_field.text(), 0)
            out_width  = int(self.cam_res_out_width_field.text(), 0)
            out_height = int(self.cam_res_out_height_field.text(), 0)
            tot_width  = int(self.cam_res_tot_width_field.text(), 0)
            tot_height = int(self.cam_res_tot_height_field.text(), 0)

            self.obc_assistant.execute(OBCQTRequest(
                lambda obc: obc.camera_set_resolution(win_x, win_y, win_width, win_height, out_width, out_height, tot_width, tot_height),
                self.handle_cmd_resp
            ))
        except ValueError:
            print("Error parsing fields")

    def cam_white_bal(self):
        try:
            is_auto = self.cam_wb_auto.isChecked()
            if is_auto:
                top_limit    = int(self.cam_wb_top_lim_field.text(), 0)
                bottom_limit = int(self.cam_wb_bot_lim_field.text(), 0)

                self.obc_assistant.execute(OBCQTRequest(
                    lambda obc: obc.camera_set_white_balance_auto_simple(top_limit, bottom_limit),
                    self.handle_cmd_resp
                ))
            else:
                gain_r = int(self.cam_wb_gain_r_field.text(), 0)
                gain_g = int(self.cam_wb_gain_g_field.text(), 0)
                gain_b = int(self.cam_wb_gain_b_field.text(), 0)

                self.obc_assistant.execute(OBCQTRequest(
                    lambda obc: obc.camera_set_white_balance_manual(gain_r, gain_g, gain_b),
                    self.handle_cmd_resp
                ))
        except ValueError:
            print("Error parsing fields")

    def cam_sde(self):
        try:
            sde_enable = self.cam_sde_enable.isChecked()
            fixed_y = (int(self.cam_sde_fixed_y_field.text(), 0)) if self.cam_sde_fixed_y.isChecked() else None
            fixed_u = (int(self.cam_sde_fixed_u_field.text(), 0)) if self.cam_sde_fixed_u.isChecked() else None
            fixed_v = (int(self.cam_sde_fixed_v_field.text(), 0)) if self.cam_sde_fixed_v.isChecked() else None
            neg_y = self.cam_sde_neg_y.isChecked()
            gray = self.cam_sde_gray.isChecked()
            solarize = self.cam_sde_solarize.isChecked()
            contrast = None
            if self.cam_sde_contrast_en.isChecked():
                contrast = (
                    int(self.cam_sde_contrast_y_offset_field.text(), 0),
                    int(self.cam_sde_contrast_y_gain_field.text(), 0),
                    int(self.cam_sde_contrast_y_bright_field.text(), 0),
                )
            saturation = None
            if self.cam_sde_sat_en.isChecked():
                saturation = (
                    int(self.cam_sde_sat_u_field.text(), 0),
                    int(self.cam_sde_sat_v_field.text(), 0),
                )
            hue = None
            if self.cam_sde_hue_en.isChecked():
                hue = (
                    int(self.cam_sde_hue_cos_field.text(), 0),
                    int(self.cam_sde_hue_sin_field.text(), 0),
                )

            self.obc_assistant.execute(OBCQTRequest(
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
                            ),
                self.handle_cmd_resp
            ))
        except ValueError:
            print("Error parsing fields")

    def handle_cmd_resp(self, resp: Union[OBCResponse, OBCError]):
        if resp is not None:
            print(resp)
            self.add_line_to_text_from_serial(str(resp))

    def handle_img(self, resp: Union[OBCResponse, OBCError]):
        # Handle errors
        if resp is None:
            return

        if not isinstance(resp, OBCResponse):
            self.add_line_to_text_from_serial(str(resp))
            return

        if resp["image_size"] <= 0:
            self.add_line_to_text_from_serial(str(resp))
            return

        # Load image data
        self.img_data = resp["image_data"]
        self.img_timestamp = datetime.now()

        self.img_pixmap = QtGui.QPixmap()
        self.img_pixmap.loadFromData(self.img_data, 'jpeg')

        self.update_img()

        self.show()

    def handle_log(self, log: OBCLog):
        line = ""

        if globs.msg_lvl_status == constants.MSG_LVL_ON:
            line += f"[{log.level.name}] "
        if globs.timestamp_status == constants.TIMESTAMPS_ON:
            line += f"[{str(log.date_time)}] "
        if globs.func_id_status == constants.FUNC_ID_ON:
            line += f"[ID: {str(log.func_id)}] "

        line += bytes_to_hexstr(log.payload, ascii_only=True)
        self.add_line_to_text_from_serial(line)

    def update_img(self):
        if self.img_pixmap is not None:
            self.image_label.setPixmap(self.img_pixmap.scaled(
                self.image_label.width(), self.image_label.height(),
                QtCore.Qt.AspectRatioMode.KeepAspectRatio)
            )

    def clear_image(self):
        self.image_label.clear()
        self.image_label.setText("No image")
        self.img_pixmap = None

    def save_image(self):
        if self.img_data is None or self.img_timestamp is None:
            return

        try:
            default_file_path = self.last_save_dir / f"ALEASAT_capture_{self.img_timestamp.strftime('%Y-%m-%d_%H%M%S')}.jpg"
            file_path, _ = QFileDialog.getSaveFileName(self,
                caption="Save Image",
                directory=str(default_file_path),
                filter="All Files (*);;JPEG Images (*.jpg)",
                initialFilter="JPEG Images (*.jpg)")

            if file_path:
                file_path = Path(file_path)
                self.last_save_dir = file_path.parent

                with open(file_path, "wb") as f:
                    f.write(self.img_data)

                print(f"Saved image to: {str(file_path)}")
        except Exception as e:
            print(e)

def main():
    app = QApplication(sys.argv)
    sa = SanAntonio()
    sa.show()
    app.exec_()

if __name__ == '__main__':
    main()
