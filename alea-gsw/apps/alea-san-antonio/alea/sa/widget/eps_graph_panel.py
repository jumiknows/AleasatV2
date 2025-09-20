from typing import Union, List
from dataclasses import dataclass

from PyQt5 import QtCore, QtWidgets

import pyqtgraph as pg

from alea.ttc import cmd_sys

from alea.sa.backend import ttcqt
from alea.sa.ui.control_panels import eps_graph_panel_ui
from alea.sa.util import console, ui_utils, ttc_utils

@dataclass
class DataSeries:
    values: List
    text_field: QtWidgets.QLineEdit
    plot_line: pg.PlotDataItem

    @staticmethod
    def create(name: str, text_field: QtWidgets.QLineEdit, plot: pg.PlotWidget, color: str) -> "DataSeries":
        plot_line = plot.plot([], [], name=name, pen=color)
        return DataSeries([], text_field, plot_line)

    def update(self, value: float, t_values: float):
        self.values.append(value)
        self.plot_line.setData(t_values, self.values)
        self.text_field.setText(f"{value:.2f}")

    def clear(self):
        self.values.clear()
        self.plot_line.setData([], self.values)
        self.text_field.setText("")

class EPSGraphPanel(QtWidgets.QWidget, eps_graph_panel_ui.Ui_EpsGraphPanel):
    def __init__(self, obc_provider: ttcqt.OBCTTCInterfaceProvider, parent=None):
        super().__init__(parent)

        self.obc_provider = obc_provider

        # Declare UI members with type hints - these are assigned in setupUI()
        # ... https://www.pythonguis.com/tutorials/plotting-pyqtgraph/

        self.setupUi(self)

        self.sample_interval_val: QtWidgets.QSpinBox
        self.measure_btn: QtWidgets.QPushButton
        self.clear_btn: QtWidgets.QPushButton
        self.voltage_panel: QtWidgets.QFrame
        self.current_panel: QtWidgets.QFrame

        # Voltage Fields
        self.v_batt: QtWidgets.QLineEdit
        self.v_bcr: QtWidgets.QLineEdit
        self.v_3v3: QtWidgets.QLineEdit
        self.v_5v: QtWidgets.QLineEdit
        self.v_3v3_lup: QtWidgets.QLineEdit
        self.v_5v_lup: QtWidgets.QLineEdit

        # Current Fields
        self.i_batt: QtWidgets.QLineEdit
        self.i_bcr: QtWidgets.QLineEdit
        self.i_3v3: QtWidgets.QLineEdit
        self.i_5v: QtWidgets.QLineEdit

        # Instantiate plots
        axis_styles = {"color": "white", "font-size": "12px", "font-weight": "bold"}

        self.voltage_plot = pg.PlotWidget()
        self.voltage_plot.setLabel("left", "Voltage (V)", **axis_styles)
        self.voltage_plot.setLabel("bottom", "Time (s)", **axis_styles)
        self.voltage_plot.setYRange(0, 5.5)
        self.voltage_legend = self.voltage_plot.addLegend()
        self.voltage_legend.setBrush('k')
        self.voltage_legend.setOffset([1, 0])
        self.voltage_plot.showGrid(x=True, y=True)
        self.voltage_panel.layout().insertWidget(1, self.voltage_plot)
        
        self.current_plot = pg.PlotWidget()
        self.current_plot.setLabel("left", "Current (mA)", **axis_styles)
        self.current_plot.setLabel("bottom", "Time (s)", **axis_styles)
        self.current_legend = self.current_plot.addLegend()
        self.current_legend.setBrush('k')
        self.current_legend.setOffset([1, 0])
        self.current_plot.showGrid(x=True, y=True)
        self.current_panel.layout().insertWidget(1, self.current_plot)

        self._timer = QtCore.QTimer(self)
        self._timer.timeout.connect(self.handle_timer)
        self._sample_interval = 0

        self.measure_btn.clicked.connect(self.handle_measure_clicked)
        self.clear_btn.clicked.connect(self.handle_clear_plots)

        # Graph Data
        self.t_values = []

        # These keys must match the respond field names in the TEST_EPS_MEASURE_BUSES command
        self.voltage_data = {
            "batt_V": DataSeries.create("Batt", self.v_batt, self.voltage_plot, 'r'),
            "bcr_V": DataSeries.create("BCR", self.v_bcr, self.voltage_plot, 'c'),
            "bus_3v3_V": DataSeries.create("3V3", self.v_3v3, self.voltage_plot, 'g'),
            "bus_5v_V": DataSeries.create("5V", self.v_5v, self.voltage_plot, 'b'),
            "lup_3v3_V": DataSeries.create("3V3 LUP", self.v_3v3_lup, self.voltage_plot, 'y'),
            "lup_5v_V": DataSeries.create("5V LUP", self.v_5v_lup, self.voltage_plot, 'm'),
        }

        self.current_data = {
            "batt_A": DataSeries.create("Batt", self.i_batt, self.current_plot, 'r'),
            "bcr_A": DataSeries.create("BCR", self.i_bcr, self.current_plot, 'c'),
            "bus_3v3_A": DataSeries.create("3V3", self.i_3v3, self.current_plot, 'g'),
            "bus_5v_A": DataSeries.create("5V", self.i_5v, self.current_plot, 'b'),
        }

    @property
    def obc(self) -> ttcqt.TTCQT:
        return self.obc_provider.obc_ttc

    @QtCore.pyqtSlot()
    def handle_measure_clicked(self):
        if self._timer.isActive():
            # Stop timer
            self._timer.stop()
        else:
            # Start timer
            if self.obc.connected:
                self._sample_interval = self.sample_interval_val.value()
                self._timer.start(self._sample_interval * 1000)
            else:
                console.print_err("Cannot start EPS measurement - not connected to OBC")

        self._update_measure_btn()

    def _update_measure_btn(self):
        if self._timer.isActive():
            self.measure_btn.setText("Stop Measuring")
            self.measure_btn.setStyleSheet(ui_utils.Color.RED.as_stylesheet())
        else:
            self.measure_btn.setText("Start Measuring")
            self.measure_btn.setStyleSheet(ui_utils.Color.GREEN.as_stylesheet())

    @QtCore.pyqtSlot()
    def handle_timer(self):
        self.obc.execute(ttcqt.TTCQTRequest(
            lambda obc: obc.eps_measure_buses(),
           self._measure_callback
        ))

    def _measure_callback(self, resp: cmd_sys.OBCResponse | Exception):
        if not ttc_utils.check_cmd_sys_resp(resp, "eps_err"):
            return

        if resp.data["eps_err"] != 0:
            console.print_err(f"EPS error during measurement: {resp.data['eps_err']}")
            return

        # Update time
        if len(self.t_values) == 0:
            self.t_values.append(0)
        else:
            self.t_values.append(self.t_values[-1] + self._sample_interval)

        # Update data
        for key, data in self.voltage_data.items():
            data.update(resp.data[key], self.t_values)

        for key, data in self.current_data.items():
            data.update(resp.data[key] * 1000, self.t_values) # Convert A to mA

    @QtCore.pyqtSlot()
    def handle_clear_plots(self):
        self.t_values.clear()

        for _, data in self.voltage_data.items():
            data.clear()

        for _, data in self.current_data.items():
            data.clear()
