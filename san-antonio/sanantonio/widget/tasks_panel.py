from typing import Union, Dict, Tuple, List
from enum import Enum

from PyQt5 import QtWidgets, QtCore

from obcpy import rtos
from obcpy.utils import exc
from obcpy.obc.system import obc_rtos_trace
from obcpy.obc.protocol.app import app_log

from sanantonio.backend import obcqt
from sanantonio.ui.control_panels import tasks_panel_ui
from sanantonio.utils import ui as ui_utils
from sanantonio.utils import console as console_utils

class TaskStatus(Enum):
    UNKNOWN        = (0, ui_utils.Color.TRANSPARENT)
    RUNNING        = (1, ui_utils.Color.TRANSPARENT)
    NOT_RESPONDING = (2, ui_utils.Color.LIGHT_RED)

    @property
    def color(self) -> ui_utils.Color:
        return self.value[1]

class TasksPanel(QtWidgets.QWidget, tasks_panel_ui.Ui_TasksPanel):
    COLUMN_STATUS      = 3
    COLUMN_STACK_USAGE = 4
    COLUMN_CPU_USAGE   = 5

    STACK_USAGE_WARN_THRESHOLD = 80

    MAXIMUM_WD_PERIOD_MS = 2500

    set_alert   = QtCore.pyqtSignal()
    clear_alert = QtCore.pyqtSignal()

    def __init__(self, obc_provider: obcqt.OBCInterfaceProvider, parent=None):
        super().__init__(parent)

        self._obc_provider = obc_provider

        # Declare UI members with type hints - these are assigned in setupUI()
        self.tasks_table: QtWidgets.QTableWidget
        self.check_stack_usage_btn: QtWidgets.QPushButton

        self.trace_table: QtWidgets.QTableWidget
        self.trace_length_box: QtWidgets.QSpinBox
        self.capture_task_trace_btn: QtWidgets.QPushButton

        # Load UI
        self.setupUi(self)

        # Task Table
        header_labels = ["ID", "Name", "Priority", "Status", "Stack Usage"]
        self.tasks_table.setColumnCount(len(header_labels))
        self.tasks_table.setHorizontalHeaderLabels(header_labels)
        self.tasks_table.horizontalHeader().setSectionResizeMode(0, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.tasks_table.horizontalHeader().setSectionResizeMode(1, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.tasks_table.horizontalHeader().setSectionResizeMode(2, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.tasks_table.horizontalHeader().setSectionResizeMode(3, QtWidgets.QHeaderView.ResizeMode.Stretch)
        self.tasks_table.horizontalHeader().setSectionResizeMode(4, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)

        self.tasks_table.setStyleSheet("QTableWidget::item { padding: 8px }")

        default_status = TaskStatus.UNKNOWN

        for i, task_spec in enumerate(self.obc.task_specs):
            self.tasks_table.insertRow(i)

            id_item = QtWidgets.QTableWidgetItem(str(task_spec.id))
            id_item.setTextAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)
            self.tasks_table.setItem(i, 0, id_item)

            self.tasks_table.setItem(i, 1, QtWidgets.QTableWidgetItem(task_spec.name))

            priority_item = QtWidgets.QTableWidgetItem(str(task_spec.priority))
            priority_item.setTextAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)
            self.tasks_table.setItem(i, 2, priority_item)

            self._set_task_status(i, default_status)
            self._set_task_stack_usage(i, task_spec, None)

        # Trace Table
        header_labels = ["Time (ms)", "ID", "Name", "Duration (ms)"]
        self.trace_table.setColumnCount(len(header_labels))
        self.trace_table.setHorizontalHeaderLabels(header_labels)
        self.trace_table.horizontalHeader().setSectionResizeMode(0, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.trace_table.horizontalHeader().setSectionResizeMode(1, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.trace_table.horizontalHeader().setSectionResizeMode(2, QtWidgets.QHeaderView.ResizeMode.Stretch)
        self.trace_table.horizontalHeader().setSectionResizeMode(3, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)

        self.trace_table.setStyleSheet("QTableWidget::item { padding: 8px }")

        # Connect signals / slots
        self.set_alert.connect(self.handle_set_alert)
        self.clear_alert.connect(self.handle_clear_alert)

        self.check_stack_usage_btn.clicked.connect(self.handle_check_stack_usage)
        self.capture_task_trace_btn.clicked.connect(self.handle_capture_task_trace)

        self._obc_provider.conn_state_changed.connect(self.handle_conn_state_changed)

        self._logs_connection: QtCore.QMetaObject.Connection = None

        self._wd_timer = QtCore.QTimer(self)
        self._wd_timer.timeout.connect(self.handle_wd_timeout)

    @property
    def obc(self) -> obcqt.OBCQT:
        return self._obc_provider.obc

    @QtCore.pyqtSlot(bool)
    def handle_conn_state_changed(self, connected: bool):
        if connected:
            self._logs_connection = self.obc.logs.connect(self.handle_log)
            self._wd_timer.start(self.MAXIMUM_WD_PERIOD_MS)
        else:
            self._wd_timer.stop()
            if self._logs_connection is not None:
                self.obc.logs.disconnect(self._logs_connection)
                self._logs_connection = None

    @QtCore.pyqtSlot(app_log.OBCLog)
    def handle_log(self, log: app_log.OBCLog):
        log_watchdog_group, log_signal_happy = self.obc.log_specs.get(group_name="LOG_WATCHDOG", signal_name="SW_WD_HAPPY")
        _, log_signal_bite = self.obc.log_specs.get(group_name="LOG_WATCHDOG", signal_name="SW_WD_BITE")

        if log.log_id == log_watchdog_group.id:
            update_task_statuses = False
            bite_mask = 0x0
            if log.signal_id == log_signal_happy.id:
                # All tasks running
                self.clear_alert.emit()
                update_task_statuses = True
            elif log.signal_id == log_signal_bite.id:
                self.set_alert.emit()
                update_task_statuses = True
                bite_mask = log.data["bite_mask"]

            if update_task_statuses:
                for i, task_spec in enumerate(self.obc.task_specs):
                    if bite_mask & (1 << task_spec.id):
                        self._set_task_status(i, TaskStatus.NOT_RESPONDING)
                    else:
                        self._set_task_status(i, TaskStatus.RUNNING)

                # Restart the timer
                self._wd_timer.start(self.MAXIMUM_WD_PERIOD_MS)

    @QtCore.pyqtSlot()
    def handle_wd_timeout(self):
        for i in range(self.obc.task_specs.count):
            self._set_task_status(i, TaskStatus.UNKNOWN)

    @QtCore.pyqtSlot()
    def handle_set_alert(self):
        pass

    @QtCore.pyqtSlot()
    def handle_clear_alert(self):
        pass

    @QtCore.pyqtSlot()
    def handle_check_stack_usage(self):
        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.get_stack_usages(),
            self._check_stack_usage_callback
        ))

    def _check_stack_usage_callback(self, resp: Union[Dict[int, Tuple[int, int]], exc.OBCError]):
        if isinstance(resp, exc.OBCError):
            console_utils.print_err(str(resp))
            return

        for i, task_spec in enumerate(self.obc.task_specs):
            if task_spec.id in resp:
                self._set_task_stack_usage(i, task_spec, resp[task_spec.id][0])

    @QtCore.pyqtSlot()
    def handle_capture_task_trace(self):
        num_ctx_switches = self.trace_length_box.value()

        # For each context switch there are two trace events
        # The first and last trace events have to be discarded
        trace_length = (num_ctx_switches + 1) * 2

        self.obc.execute(obcqt.OBCQTRequest(
            lambda obc: obc.capture_rtos_trace(trace_length),
            self._capture_task_trace_callback
        ))

    def _capture_task_trace_callback(self, resp: Union[obc_rtos_trace.OBCRTOSTrace, exc.OBCError]):
        if isinstance(resp, exc.OBCError):
            console_utils.print_err(str(resp))
            return

        rtos_trace: obc_rtos_trace.OBCRTOSTrace = resp

        try:
            task_runs = rtos_trace.to_task_runs()
        except exc.OBCTaskTraceError as e:
            console_utils.print_err(f"Task trace corrupted: {str(e)}")
            return

        self._display_task_runs(task_runs)

    def _set_task_status(self, row: int, status: TaskStatus):
        item = self.tasks_table.item(row, self.COLUMN_STATUS)

        if item is None:
            item = QtWidgets.QTableWidgetItem()
            self.tasks_table.setItem(row, self.COLUMN_STATUS, item)

        item.setText(status.name)
        item.setBackground(status.color.as_qcolor())

    def _set_task_stack_usage(self, row: int, task_spec: rtos.task_spec.OBCTaskSpec, usage: int):
        item = self.tasks_table.item(row, self.COLUMN_STACK_USAGE)

        if item is None:
            item = QtWidgets.QTableWidgetItem()
            self.tasks_table.setItem(row, self.COLUMN_STACK_USAGE, item)

        if usage is None:
            usage          = "-"
            usage_pcnt     = None
            usage_pcnt_str = ""
        else:
            usage_pcnt     = int(round((usage / task_spec.stack_size) * 100, 0))
            usage_pcnt_str = f" ({usage_pcnt}%)"
        item.setText(f"{usage} / {task_spec.stack_size}{usage_pcnt_str}")

        if usage_pcnt is None:
            item.setBackground(ui_utils.Color.TRANSPARENT.as_qcolor())
        else:
            if usage_pcnt >= 100:
                item.setBackground(ui_utils.Color.LIGHT_RED.as_qcolor())
            if usage_pcnt >= self.STACK_USAGE_WARN_THRESHOLD:
                item.setBackground(ui_utils.Color.DARK_ORANGE.as_qcolor())
            else:
                item.setBackground(ui_utils.Color.TRANSPARENT.as_qcolor())

    def _display_task_runs(self, task_runs: List[obc_rtos_trace.OBCTaskRun]):
        # Remove all rows
        self.trace_table.setRowCount(0)

        # Add row for each task run
        for task_run in task_runs:
            try:
                task_name = self.obc.task_specs.get(id=task_run.task_id).name
            except exc.OBCTaskNotFoundError as e:
                console_utils.print_err(f"Task trace corrupted: {str(e)}")
                continue

            start_time_ms = round(task_run.start_time_us / 1000, 3)
            duration_ms   = round(task_run.duration_us / 1000, 3)

            row = self.trace_table.rowCount()
            self.trace_table.insertRow(row)

            self.trace_table.setItem(row, 0, QtWidgets.QTableWidgetItem(str(start_time_ms)))

            id_item = QtWidgets.QTableWidgetItem(str(task_run.task_id))
            id_item.setTextAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)
            self.trace_table.setItem(row, 1, id_item)

            self.trace_table.setItem(row, 2, QtWidgets.QTableWidgetItem(task_name))
            self.trace_table.setItem(row, 3, QtWidgets.QTableWidgetItem(str(duration_ms)))

            # Color IDLE task
            if task_run.task_id == 0:
                for col in range(self.trace_table.columnCount()):
                    self.trace_table.item(row, col).setBackground(ui_utils.Color.BLACK.as_qcolor())
