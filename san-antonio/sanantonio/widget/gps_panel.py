from typing import Union
from PyQt5 import QtWidgets, QtCore
import struct

from obcpy import cmd_sys
from obcpy.utils import exc
import obcpy.obc.gps.gps_utils as UTILS

from sanantonio.backend import obcqt
from sanantonio.ui.control_panels import gps_panel_ui
from sanantonio.utils import obc as obc_utils


class GpsPanel(QtWidgets.QWidget, gps_panel_ui.Ui_GpsPanel):
    def __init__(self, obc_provider: obcqt.OBCInterfaceProvider, parent=None):
        super().__init__(parent)

        self._obc_provider = obc_provider

        # Declare UI members with type hints - these are assigned allocated in setupUI()
        self.gps_get_pwr_mode_btn: QtWidgets.QPushButton
        self.gps_pwr_mode_edit: QtWidgets.QLineEdit
        self.gps_pwr_save_check: QtWidgets.QCheckBox
        self.gps_pwr_write_box: QtWidgets.QComboBox
        self.gps_set_pwr_mode_btn: QtWidgets.QPushButton
        self.gps_get_software_crc_btn: QtWidgets.QPushButton
        self.gps_software_crc_mode_edit: QtWidgets.QLineEdit
        self.gps_get_software_ver_btn: QtWidgets.QPushButton
        self.gps_restart_btn: QtWidgets.QPushButton
        self.gps_restart_mode_box: QtWidgets.QComboBox
        self.gps_get_pos_update_btn: QtWidgets.QPushButton
        self.gps_pos_update_edit: QtWidgets.QLineEdit
        self.gps_set_pos_update_btn: QtWidgets.QPushButton
        self.gps_pos_update_write_box: QtWidgets.QComboBox
        self.gps_pos_update_rate_box: QtWidgets.QComboBox
        self.gps_get_nav_interval_btn: QtWidgets.QPushButton
        self.gps_nav_interval_edit: QtWidgets.QLineEdit
        self.gps_set_nav_interval_btn: QtWidgets.QPushButton
        self.gps_nav_interval_write_box: QtWidgets.QComboBox
        self.gps_set_nav_interval_edit: QtWidgets.QLineEdit
        self.gps_get_nmea_settings_btn: QtWidgets.QPushButton
        self.gps_set_nav_interval_btn: QtWidgets.QPushButton
        self.gps_set_nmea_settings_btn: QtWidgets.QPushButton
        self.gps_get_nmea_settings_gga_line: QtWidgets.QLineEdit
        self.gps_set_nmea_settings_gga_line: QtWidgets.QLineEdit
        self.gps_get_nmea_settings_gll_line: QtWidgets.QLineEdit
        self.gps_set_nmea_settings_gll_line: QtWidgets.QLineEdit
        self.gps_get_nmea_settings_gsv_line: QtWidgets.QLineEdit
        self.gps_set_nmea_settings_gsv_line: QtWidgets.QLineEdit
        self.gps_get_nmea_settings_rmc_line: QtWidgets.QLineEdit
        self.gps_set_nmea_settings_rmc_line: QtWidgets.QLineEdit
        self.gps_get_nmea_settings_vtg_line: QtWidgets.QLineEdit
        self.gps_set_nmea_settings_vtg_line: QtWidgets.QLineEdit
        self.gps_get_nmea_settings_zda_line: QtWidgets.QLineEdit
        self.gps_set_nmea_settings_zda_line: QtWidgets.QLineEdit
        self.gps_set_nmea_settings_box: QtWidgets.QComboBox

        self.setupUi(self)

        # Connect signals / slots
        self.gps_get_pwr_mode_btn.clicked.connect(self.handle_get_pwr_mode)
        self.gps_set_pwr_mode_btn.clicked.connect(self.handle_set_pwr_mode)
        self.gps_get_software_crc_btn.clicked.connect(
            self.handle_get_software_crc)
        self.gps_restart_btn.clicked.connect(self.handle_restart)
        self.gps_get_software_ver_btn.clicked.connect(
            self.handle_get_software_ver)
        self.gps_get_pos_update_btn.clicked.connect(
            self.handle_get_position_update_rate)
        self.gps_set_pos_update_btn.clicked.connect(
            self.handle_set_position_update_rate)
        self.gps_set_nav_interval_btn.clicked.connect(
            self.handle_set_navigation_msg_int)
        self.gps_get_nmea_settings_btn.clicked.connect(
            self.handle_get_nmea_settings)
        self.gps_set_nmea_settings_btn.clicked.connect(
            self.handle_set_nmea_settings)

    @property
    def obc(self) -> obcqt.OBCQT:
        return self._obc_provider.obc

    @QtCore.pyqtSlot()
    def handle_get_pwr_mode(self):
        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.gps_get_pwr_mode(),
            self._capture_pwr_mode_callback
        ))

    def _capture_pwr_mode_callback(self, resp: Union[cmd_sys.resp.OBCResponse, exc.OBCError]):
        if not obc_utils.check_cmd_sys_resp(resp, "gps_in_power_save_mode"):
            return

        if resp.data["gps_in_power_save_mode"]:
            self.gps_pwr_mode_edit.setText("Power Save")
        else:
            self.gps_pwr_mode_edit.setText("Regular")

    @QtCore.pyqtSlot()
    def handle_set_pwr_mode(self):
        pwr_mode = self.gps_pwr_save_check.isChecked()
        write_mode = UTILS.WriteSetting.get(
            self.gps_pwr_write_box.currentIndex())

        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.gps_set_pwr_mode(write_mode, pwr_mode)
        ))

    @QtCore.pyqtSlot()
    def handle_get_software_crc(self):
        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.gps_get_software_crc(),
            self._capture_software_crc_callback
        ))

    def _capture_software_crc_callback(self, resp: Union[cmd_sys.resp.OBCResponse, exc.OBCError]):
        if not obc_utils.check_cmd_sys_resp(resp, "software_crc"):
            return

        self.gps_software_crc_mode_edit.setText(str(resp.data["software_crc"]))

    @QtCore.pyqtSlot()
    def handle_restart(self):
        restart_mode = UTILS.RestartMode.get(
            self.gps_restart_mode_box.currentIndex())

        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.gps_restart(restart_mode),
        ))

    @QtCore.pyqtSlot()
    def handle_get_software_ver(self):
        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.gps_get_software_ver(),
            self._capture_software_ver_callback
        ))

    def _capture_software_ver_callback(self, resp: Union[cmd_sys.resp.OBCResponse, exc.OBCError]):
        if not obc_utils.check_cmd_sys_resp(resp, "gps_software_version"):
            return

        version_data = resp.data["gps_software_version"]
        string_numbers = [str(num) for num in version_data]

        # Split the list into groups of 3 numbers
        groups = [string_numbers[i:i+3]
                  for i in range(0, len(string_numbers), 3)]

        # Join each group with dots and join the groups with dashes
        software_version = '-'.join(['.'.join(group) for group in groups])

        self.gps_software_ver_mode_edit.setText(software_version)

    @QtCore.pyqtSlot()
    def handle_get_position_update_rate(self):
        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.gps_get_position_update_rate(),
            self._capture_position_update_rate
        ))

    def _capture_position_update_rate(self, resp: Union[cmd_sys.resp.OBCResponse, exc.OBCError]):
        if not obc_utils.check_cmd_sys_resp(resp, "update_rate"):
            return

        update_rate = resp.data["update_rate"]
        self.gps_pos_update_edit.setText(str(update_rate)+"Hz")

    @QtCore.pyqtSlot()
    def handle_set_position_update_rate(self):
        write_mode = UTILS.WriteSetting.get(
            self.gps_pos_update_write_box.currentIndex())
        position_update_rates = [1, 2, 4, 5, 8, 10, 20, 25, 40, 50]
        pos_update_rate = position_update_rates[self.gps_pos_update_rate_box.currentIndex(
        )]
        if pos_update_rate >= 20:
            print(
                "Selected position update rate is too low. Please consider raising serial baud rate.")

        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.gps_set_position_update_rate(
                write_mode, pos_update_rate)
        ))

    @QtCore.pyqtSlot()
    def handle_set_navigation_msg_int(self):
        write_mode = UTILS.WriteSetting.get(
            self.gps_nav_interval_write_box.currentIndex())
        msg_int = int(self.gps_set_nav_interval_edit.text())
        if msg_int < 0 or msg_int > 255:
            raise Exception("Nav interval must be (0,255]")

        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.gps_set_nav_msg_interval(write_mode, msg_int),
        ))

    @QtCore.pyqtSlot()
    def handle_set_nmea_settings(self):
        write_mode = UTILS.WriteSetting.get(
            self.gps_set_nmea_settings_box.currentIndex())
        gga = int(self.gps_set_nmea_settings_gga_line.text())
        gsa = int(self.gps_set_nmea_settings_gsa_line.text())
        gsv = int(self.gps_set_nmea_settings_gsv_line.text())
        gll = int(self.gps_set_nmea_settings_gll_line.text())
        rmc = int(self.gps_set_nmea_settings_rmc_line.text())
        vtg = int(self.gps_set_nmea_settings_vtg_line.text())
        zda = int(self.gps_set_nmea_settings_zda_line.text())

        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.gps_set_nmea_settings(
                write_mode, gga, gsa, gsv, gll, rmc, vtg, zda),
        ))

    @QtCore.pyqtSlot()
    def handle_get_nmea_settings(self):

        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.gps_get_nmea_settings(),
            self._capture_nmea_settings
        ))

    def _capture_nmea_settings(self, resp: Union[cmd_sys.resp.OBCResponse, exc.OBCError]):
        if not obc_utils.check_cmd_sys_resp(resp, "gga_interval") or \
                not obc_utils.check_cmd_sys_resp(resp, "gsa_interval") or \
                not obc_utils.check_cmd_sys_resp(resp, "gsv_interval") or \
                not obc_utils.check_cmd_sys_resp(resp, "gll_interval") or \
                not obc_utils.check_cmd_sys_resp(resp, "rmc_interval") or \
                not obc_utils.check_cmd_sys_resp(resp, "vtg_interval") or \
                not obc_utils.check_cmd_sys_resp(resp, "zda_interval"):
            return

        self.gps_get_nmea_settings_gga_line.setText(
            str(resp.data["gga_interval"]))
        self.gps_get_nmea_settings_gsa_line.setText(
            str(resp.data["gsa_interval"]))
        self.gps_get_nmea_settings_gsv_line.setText(
            str(resp.data["gsv_interval"]))
        self.gps_get_nmea_settings_gll_line.setText(
            str(resp.data["gll_interval"]))
        self.gps_get_nmea_settings_rmc_line.setText(
            str(resp.data["rmc_interval"]))
        self.gps_get_nmea_settings_vtg_line.setText(
            str(resp.data["vtg_interval"]))
        self.gps_get_nmea_settings_zda_line.setText(
            str(resp.data["zda_interval"]))
