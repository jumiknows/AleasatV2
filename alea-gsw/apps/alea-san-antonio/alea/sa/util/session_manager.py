import datetime
import json

from PyQt5 import QtWidgets

class SessionManager:
    def __init__(self, main_window):
        self.main_window = main_window
    
    def new_session_helper(self):
        self.main_window.obc_serial_log.handle_clear()
        self.main_window.san_antonio_log.clear_output()
        self.main_window.obc_cmd_resp_panel.immediate.clear_output()
        self.main_window.control_panels.hiltest_panel.reset_hiltestpanel()
        # self.main_window.control_panels.eps_panel.reset_eps_panel()
        # self.main_window.control_panels.eps_graph_panel.reset_graph_panel()

    def save_session_helper(self):
        # Get file location from user
        file_name = QtWidgets.QFileDialog.getSaveFileName(self.main_window, 'Save Session', '', 'JSON Files (*.json)')[0]

        if file_name == '':
            print("No file selected")
            return
        
        session_json = {"session_data": {"date_created": datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")}}

        session_json['obc_cmd_resp_imm_table'] = []
        obc_cmd_resp_imm_table = self.main_window.obc_cmd_resp_panel.immediate.obc_cmd_resp_imm_table

        for row in range(obc_cmd_resp_imm_table.rowCount()):
            row_data = []
            for column in range(obc_cmd_resp_imm_table.columnCount()):
                item = obc_cmd_resp_imm_table.item(row, column)
                if item is not None:
                    row_data.append(item.text())
                else:
                    row_data.append("")
            session_json['obc_cmd_resp_imm_table'].append(row_data)  
 
        session_json['obc_serial_log_table'] = []
        obc_serial_log_table = self.main_window.obc_serial_log.obc_log_table

        for row in range(obc_serial_log_table.rowCount()):
            row_data = []
            for column in range(obc_serial_log_table.columnCount()):
                item = obc_serial_log_table.item(row, column)
                if item is not None:
                    row_data.append(item.text())
                else:
                    row_data.append("")
                session_json['obc_serial_log_table'].append(row_data)

        # session_json['eps_data'] = self.main_window.control_panels.eps_panel.eps_data
        
        session_json['san_antonio_log_text'] = self.main_window.san_antonio_log.sa_log_text.toPlainText()

        # session_json['eps_graph_data'] = self.main_window.control_panels.eps_graph_panel.graphData
        # session_json['eps_graph_count'] = self.main_window.control_panels.eps_graph_panel.graphDataCount

        HIL_test_history = self.main_window.control_panels.hiltest_panel.getJSONinfo()
        session_json['HIL_test_history'] = HIL_test_history

        try:
            with open(file_name, 'w') as outfile:
                json.dump(session_json, outfile, indent=4)

            print(f"Saved to: {file_name}")
        # only file based exceptions are caught here, os based exceptions are not caught
        except (FileNotFoundError, PermissionError, FileExistsError) as e:
            print(f"Error saving session: {e}")

    def load_session_helper(self):
        total_time = datetime.datetime.now()
        # Get file location from user
        file_name = QtWidgets.QFileDialog.getOpenFileName(self.main_window, 'Load Session', '', 'JSON Files (*.json)')[0]

        if file_name == '':
            print("No file selected")
            return
        
        try:
            with open(file_name) as json_file:
                session_json = json.load(json_file)
        except (FileNotFoundError, PermissionError, FileExistsError) as e:
            print(f"Error loading session: {e}")
            return

        
        self.new_session_helper() # clear the current session

        try:
            self._load_resp_imm_table(session_json['obc_cmd_resp_imm_table'])
        except KeyError:
            print("No immediate commands found in session file")
            
        try:
            self._load_serial_log_table(session_json['obc_serial_log_table'])
        except KeyError:
            print("No serial log found in session file")

        try:
            self._load_sa_log_text(session_json['san_antonio_log_text'])
        except KeyError:
            print("No San Antonio log found in session file")

        try:
            self._load_eps_data(session_json['eps_data'])
        except KeyError:
            print("No EPS data found in session file")

        try:
            self._load_eps_graph(session_json['eps_graph_data'], session_json['eps_graph_count'])
        except KeyError:
            print("No EPS graph data found in session file")

        try:
            self._load_hil_tests(session_json['HIL_test_history'])
        except KeyError:
            print("No HIL test history found in session file")


        print(f"Loaded from: {file_name}")

    def _load_eps_data(self, eps_data):
        self.main_window.control_panels.eps_panel.eps_data = eps_data
        self.main_window.control_panels.eps_panel.write_status()

    def _load_eps_graph(self, eps_graph_data, eps_graph_count):
        self.main_window.control_panels.eps_graph_panel.graphData = eps_graph_data
        self.main_window.control_panels.eps_graph_panel.graphDataCount = eps_graph_count
        self.main_window.control_panels.eps_graph_panel.plotGraphs()

    def _load_resp_imm_table(self, obc_serial_log):
        table = self.main_window.obc_cmd_resp_panel.immediate.obc_cmd_resp_imm_table
        table.setUpdatesEnabled(False)  # Disable table updates

        for index, row in enumerate(obc_serial_log):
            row_count = table.rowCount()
            table.insertRow(row_count)
            for column, item in enumerate(row):
                table.setItem(row_count, column, QtWidgets.QTableWidgetItem(item))
            table.resizeRowToContents(row_count)

        table.setUpdatesEnabled(True)  # Re-enable updates
        table.resizeColumnsToContents()  # Adjust column size after filling

    def _load_serial_log_table(self, obc_serial_log):
        table = self.main_window.obc_serial_log.obc_log_table
        table.setUpdatesEnabled(False) # Disable table updates

        for index, row in enumerate(obc_serial_log):
            row_count = table.rowCount()
            table.insertRow(row_count)
            for column, item in enumerate(row):
                table.setItem(row_count, column, QtWidgets.QTableWidgetItem(item))
            table.resizeRowToContents(row_count)
        
        table.setUpdatesEnabled(True) # Re-enable updates
        table.resizeColumnsToContents() # Adjust column size after filling

    def _load_hil_tests(self, hiltest_data): 
        # handle load session in panel file 
        self.main_window.control_panels.hiltest_panel.loadJSONinfo(hiltest_data)

    def _load_sa_log_text(self, san_antonio_log):
        print(san_antonio_log) # Slightly jank but it works since the print statement is redirected to the SA log
