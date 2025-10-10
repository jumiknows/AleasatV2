from PyQt5 import QtWidgets, QtCore

from alea.sa.backend import ttcqt
from alea.sa.ui.control_panels import hil_test_panel_ui
from alea.ttc import cmd_sys
from alea.sa.util import ttc_utils

# NEW IMPORTS COMPARED TO OTHER PANELS
from os.path import exists

from pathlib import Path
from datetime import datetime
import unittest
import json

from alea.sa.util import git_utils
from alea.test.hil import hil_test
import alea.test.hil.suites

import unittest

class HILTestsPanel(QtWidgets.QWidget, hil_test_panel_ui.Ui_HILTestsPanel):
    loader = unittest.TestLoader()
    suites_dir = Path(alea.test.hil.suites.__file__).parent 
    test_suite = loader.discover(suites_dir, pattern="*_test.py")

    hil_test_classes = []

    num_HIL_tests = 0
    num_all_tests = 0
    test_nums = {}

    for i in range(len(test_suite._tests)):
        tests = test_suite._tests[i]
        for j in range(len(tests._tests)):
            tests2 = tests._tests[j]
            test_class_end_index = str(tests2._tests).find(' ')
            test_class_start_index = str(tests2._tests).find('t.')
            test_class = str(tests2._tests)[test_class_start_index + 2:test_class_end_index - 4]
            hil_test_classes.append(test_class)
            num_HIL_tests += 1
            num_all_tests += len(tests2._tests)
            num_tests = str(len(tests2._tests))
            test_nums.update({test_class : num_tests})

    hil_test_classes.sort(key=lambda x: x[0])

    # Top DataRow variables
    tests_ran_global = 0 # Default value before running tests 
    tests_passed = 0 # Default value before any tests actually pass 
    current_HIL_test = 'No HIL test yet' 

    run_all_tests_failures = 0
    # Variables for run_all_tests()
    running_all_tests = False
    num_all_tests_ran = 0 

    # hilTestTable variables 
    status_buttons = [None]*len(hil_test_classes)

    # JSON attributes
    # Put in sanantonio directory (for now)
    JSON_history_dir = Path(alea.sa.__file__).parent
    JSON_path = JSON_history_dir / 'SanAntonioTestLog.JSON'

    test_info = ["Number of tests", "Tests ran", "Time taken (seconds)", "Time last ran (local)", 
                "Error logs", "Status", "Instance"]
    data_info = ["DataRow variables", "HIL tests", "Total tests", "Tests ran", "Tests passed", 
                "Current HIL test", "Number of reds"]

    existing_JSON = exists(str(JSON_path))

    set_alert = QtCore.pyqtSignal()

    def __init__(self, obc_provider: ttcqt.OBCTTCInterfaceProvider, parent=None):
        super().__init__(parent)

        self._obc_provider = obc_provider
        
        if not self.existing_JSON:
            history_list = self.create_JSON()
            self.existing_JSON = True
        
        # DataRow 

        # Declare UI members with type hints - these are assigned in setupUI()
        self.HILTestsPanel: QtWidgets.QWidget

        self.DataRow: QtWidgets.QTableWidget
        self.RunTests: QtWidgets.QPushButton
        self.Status : QtWidgets.QPushButton # Fake button, doesn't actually do anything when pressed  
                                            # Just changes appearance to indicate test progress 
        # Load UI
        self.setupUi(self)

        self.DataRow.setShowGrid(False)

        # Initalizing table elements
        # 1st column, Number of HIL tests
        self.set_data_row_text(0, str(self.num_HIL_tests))
        # 2nd column, Number of tests
        self.set_data_row_text(1, str(self.num_all_tests))
        # 3rd column, Tests ran (NOT number of HIL tests, but all tests)
        self.set_data_row_text(2, str(self.tests_ran_global))
        # 4th column, Tests passed 
        self.set_data_row_text(3, str(self.tests_passed))
        # 5th column, Current HIL test 
        self.set_data_row_text(4, self.current_HIL_test)

        # Status Button 
        # Need to make circle, height and width of button is 56 px 
        self.Status.setStyleSheet("border-radius : 28; border : 1px solid black; background-color: #00000000") # 56/2 is 28

        # Run all tests Button 
        # Link to run_all_tests()
        self.RunTests.clicked.connect(self.run_all_tests)

        # Strech to fit in header
        for i in range(self.DataRow.columnCount()):
            self.DataRow.horizontalHeader().setSectionResizeMode(i, QtWidgets.QHeaderView.ResizeMode.Stretch)

        # Lastly, clear current DataRow Variables on init
        for i in range(1, len(self.data_info)):
            self.write_JSON_value('DataRow variables', self.data_info[i], '')

        # HILTestTable

        self.HILTestsList: QtWidgets.QScrollArea
        self.HILTestsListContents : QtWidgets.QWidget
        self.HILTestsTable : QtWidgets.QTableWidget 

        header_labels = ["Name", "Number of tests", "Tests ran", "Time taken (seconds)", 
                         "Time last ran (local)", "Previous run", "Status", "Run test"]
        
        self.HILTestsTable.setColumnCount(len(header_labels))
        self.HILTestsTable.setHorizontalHeaderLabels(header_labels)

        # Strech headers  
        self.HILTestsTable.horizontalHeader().setSectionResizeMode(0, QtWidgets.QHeaderView.ResizeMode.Stretch)
        self.HILTestsTable.horizontalHeader().setSectionResizeMode(1, QtWidgets.QHeaderView.ResizeMode.Stretch)
        self.HILTestsTable.horizontalHeader().setSectionResizeMode(2, QtWidgets.QHeaderView.ResizeMode.Stretch)
        self.HILTestsTable.horizontalHeader().setSectionResizeMode(5, QtWidgets.QHeaderView.ResizeMode.Stretch)
        # Resize headers 
        self.HILTestsTable.horizontalHeader().setSectionResizeMode(3, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.HILTestsTable.horizontalHeader().setSectionResizeMode(4, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.HILTestsTable.horizontalHeader().setSectionResizeMode(6, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)
        self.HILTestsTable.horizontalHeader().setSectionResizeMode(7, QtWidgets.QHeaderView.ResizeMode.ResizeToContents)

        # Initalized in this for loop:
            # 1st column, names of test file
            # 2nd column, number of tests in file
            # 3rd column, number of tests ran 
            # 4th column, length of time HIL test took 
            # 5th column, last time ran
            # 6th column, status of test
            # 7th column, button to run tests
        for test in self.hil_test_classes:
            test_index = self.hil_test_classes.index(test)
            self.HILTestsTable.insertRow(test_index)

            # 1) Name of test file
            # This is computed in hilTests[]
            self.set_table_text(test_index, 0, test)

            # 2) Number of tests in test file 
            num_tests = str(self.test_nums[test])
            self.set_table_text(test_index, 1, num_tests)
            self.write_JSON_value(test, 'Number of tests', num_tests) # Write as this doesn't change
            self.write_JSON_value(test, 'Instance', '') # Reset current instance on __init__

            tests_ran = self.get_JSON_member(test, 'Tests ran')
            test_has_ran = len(tests_ran) > 0

            # 3) Total number of tests ran
            if test_has_ran: 
                self.set_table_text(test_index, 2, tests_ran)
            else: 
                self.set_table_text(test_index, 2, str(0)) # On init it's 0 as we've ran no tests yet

            # 4) Length of time HIL test took
            time_taken = self.get_JSON_member(test, 'Time taken (seconds)')

            if test_has_ran: 
                self.set_table_text(test_index, 3, time_taken)
            else: 
                self.set_table_text(test_index, 3, str(0))

            # 5) Last time test was run
            last_time_ran = self.get_JSON_member(test, 'Time last ran (local)')

            if test_has_ran: 
                self.set_table_text(test_index, 4, last_time_ran)
            else: 
                self.set_table_text(test_index, 4, "YYYY-MM-DD hh:mm:ss")

            # 6) Previous test result 
            if test_has_ran: # if test has been run before

                previous_status = self.get_JSON_member(test, 'Status')
                
                if(len(previous_status) > 0): # There are error logs
                    red_text = previous_status + ' failed'
                    self.set_table_text(test_index, 5, red_text)
                else:
                    self.set_table_text(test_index, 5, 'Succeeded')

            else: # Tests haven't run before  
                self.set_table_text(test_index, 5, "N/A")

            # 7) Status of test
            # Set to transparent on init
            # Has to be dealt with more care for prettiness 
            StatusButton = QtWidgets.QPushButton()

            def create_print_status(test_name): # This function is needed to perserve the value pass of testClass through lambda
                return lambda: self.print_status_info(test_name)

            # Status button will display error logs when clicked
            StatusButton.clicked.connect(create_print_status(test))

            row_height = int(self.HILTestsTable.rowHeight(test_index))

            StatusButton.setFixedSize(row_height, row_height) 
            StatusButton.setStyleSheet("border-radius : "+str(int(row_height/2))+"; border : 1px solid black; background-color: #00000000")

            # This container is to center the button in cell 
            StatusButtonContainer = QtWidgets.QWidget()
            StatusButtonLayout = QtWidgets.QHBoxLayout(StatusButtonContainer)
            StatusButtonLayout.addStretch() # Centers left
            StatusButtonLayout.addWidget(StatusButton)
            StatusButtonLayout.addStretch() # Centers right
            StatusButtonLayout.setContentsMargins(0, 0, 0, 0) # Centers vertically
            StatusButtonContainer.setLayout(StatusButtonLayout)

            self.HILTestsTable.setCellWidget(test_index, 6, StatusButtonContainer)

            # Store in array to be referenced again during execution
            # (Change appearance during tests running)
            self.status_buttons[test_index] = StatusButton

            # 8) Button to run test
            # Done manually without set_table_text() as this is a button, not text
            test_button_text = 'Run ' + test + ' test'
            TestButton = QtWidgets.QPushButton(test_button_text)
            self.HILTestsTable.setCellWidget(test_index, 7, TestButton)

            TestButton.clicked.connect(self.create_test_button(test)) 
        
        self.set_alert.connect(self.handle_set_alert)

    @property
    def obc(self) -> ttcqt.TTCQT:
        return self._obc_provider.obc_ttc
    
    # Used in 8th column, button to run tests
    # This function definition may seem redunant i.e. why don't you just put lambda : self.run_test(testClass) in connect()? 
    # without this function run_test() doesn't work in the for loop, won't be able to pass back values to process_results
    @QtCore.pyqtSlot()
    def create_test_button(self, test):
        return lambda : self.run_test(test)  

    # run any test given testClass
    # This function and all the functions it links to only works when OBC connected in SA
    @QtCore.pyqtSlot()
    def run_test(self, test):
        if self.obc.connected:
            # composite function used to pass back results  
            # get_results called ONLY from execute(), which is why it's local
            def get_results(obc):
                red_results, green_results, time_taken = hil_test.runner_for_SA(obc, test)
                last_time_ran = str(datetime.now())[:-7] # Remove microseconds for YYYY-MM-DD hh:mm:ss format

                self.process_results(test, red_results, green_results, time_taken, last_time_ran)

            # Update current HIL test in DataRow
            self.current_HIL_test = test + ' test'
            if self.running_all_tests:
                self.current_HIL_test = 'Running all tests'
                
            self.set_data_row_text(4, self.current_HIL_test) 
            # Update status button for test to 'running'
            self.change_test_button(test, 'yellow', 0)

            self.obc.execute(ttcqt.TTCQTRequest(get_results))

    @QtCore.pyqtSlot()
    def run_all_tests(self): 
        if self.obc.connected:
            self.change_status_button('yellow', 0)
            # Clear any old execution
            self.tests_ran_global = 0
            self.tests_passed = 0
            self.current_HIL_test = 'Running all tests' # All tests running 

            self.num_all_tests_ran = 0
            
            self.set_data_row_text(2, str(self.tests_ran_global)) # 3rd column, Tests ran (NOT number of HIL tests, but all tests)
            self.set_data_row_text(3, str(self.tests_passed)) # 4th column, Tests passed 
            self.set_data_row_text(4, self.current_HIL_test)

            self.run_all_tests_failures = 0
            self.running_all_tests = True

            for test in self.hil_test_classes:
                self.run_test(test) # All updates need to be in process_results because of threads

    # process_results() updates test info after execution and is linked to obc test runner 
    # This is where all of the dynamic updates to test info needs to be 
    # updates:
        # Top DataRow
            # 3rd column, Tests ran (NOT number of HIL tests, but all tests)
            # 4th column, Tests passed 
            # 5th column, Current HIL test 
        # HILTestsTable
            # 3rd column, number of tests ran 
            # 4th column, length of time HIL test took 
            # 5th column, last time ran
            # 6th column, previous test result
            # 7th column, status of test
        # Also writes all JSON results
    @QtCore.pyqtSlot(str, list, list, float, str)
    def process_results(self, test, red_results, green_results, time_taken, last_time_ran):
        test_index = self.hil_test_classes.index(test)
        # DataRow
        # 3rd column, Tests ran (NOT number of HIL tests, but all tests)
        # Increment by len(results)
        self.tests_ran_global = self.tests_ran_global + len(red_results + green_results)
        self.set_alert.emit()
        self.set_data_row_text(2, str(self.tests_ran_global))

        # 4th column, Tests passed
        self.tests_passed = self.tests_passed + len(green_results)
        self.set_data_row_text(3, str(self.tests_passed))

        # 5th column, Current HIL test
        self.current_HIL_test = 'No HIL test' # No test running (finished execution)
        if self.running_all_tests:
            self.current_HIL_test = 'Running all tests' 
            self.num_all_tests_ran = self.num_all_tests_ran + 1

            if self.num_all_tests_ran == len(self.hil_test_classes): # All tests complete 
                self.current_HIL_test = 'Ran all tests' # No test running (finished execution)
                self.running_all_tests = False
        
        self.set_data_row_text(4, self.current_HIL_test)

        # Now let's also update DataRow JSON for current instance
        self.write_JSON_value('DataRow variables', 'HIL tests', str(self.num_HIL_tests))
        self.write_JSON_value('DataRow variables', 'Total tests', str(self.num_all_tests))
        self.write_JSON_value('DataRow variables', 'Tests ran', str(self.tests_ran_global))
        self.write_JSON_value('DataRow variables', 'Tests passed', str(self.tests_passed))
        self.write_JSON_value('DataRow variables', 'Current HIL test', self.current_HIL_test)

        num_reds = len(red_results) 
        if num_reds == 0:
            pass # Doesn't need to be changed
        else:
            num_reds_prev = self.get_JSON_member('DataRow variables', 'Number of reds')
            if len(num_reds_prev) == 0:
                num_reds_prev = 0
            new_reds = int(num_reds_prev) + len(red_results)
            self.write_JSON_value('DataRow variables', 'Number of reds', str(new_reds))

        # HILTestsTable
        # 3rd column, number of tests ran
        tests_ran_previous = self.get_JSON_member(test, 'Tests ran')
        test_has_ran = len(tests_ran_previous) > 0 # Has this test been run before?

        tests_ran = str(len(red_results + green_results))
        self.set_alert.emit()
        self.set_table_text(test_index, 2, tests_ran)
        self.write_JSON_value(test, 'Tests ran', tests_ran)

        # 4th column, length of time HIL test took
        self.set_table_text(test_index, 3, str(time_taken)[:5]) 
        self.write_JSON_value(test, 'Time taken (seconds)', str(time_taken)[:5])

        # 5th column, last time ran
        self.set_table_text(test_index, 4, str(last_time_ran))
        self.write_JSON_value(test, 'Time last ran (local)', str(last_time_ran))

        # 6th column, previous test result
        if test_has_ran: # if test has been run before 
            previous_status = self.get_JSON_member(test, 'Status')
        
            if(len(previous_status) > 0): # There were error logs in previous test
                red_text = previous_status + ' failed'
                self.set_table_text(test_index, 5, red_text)
            else:
                self.set_table_text(test_index, 5, 'Succeeded')

        else: # Test N/A  
            # set it to "N/A" as there was no previous 
            self.set_table_text(test_index, 5, "N/A")

        # 7th column, status of test
        if len(red_results) != 0:
            self.change_test_button(test, 'red', len(red_results)) # Update for specific test
            self.run_all_tests_failures = len(red_results) + self.run_all_tests_failures # Update for RunAll

            self.change_status_button('red', self.run_all_tests_failures)
        
            # We don't want to write entire traceback of red_results, we only want the test that failed, line # and assertion error
            # All that info is right after traceback mention of file name 
            # ex: ALOT OF TEXT...\obc_tests\\epsTest.py", line 32, in test_eps_read_float_battery_voltage\n    self.assertEqual(resp.data["eps_err"], 0)\nAssertionError: -1 != 0\n'
            for i in range(len(red_results)):
                file_name_index = red_results[i][2].find(test.lower())
                # find() will return lowest index, so we increment by len(fileName) + 3 for ", 
                file_name_index = file_name_index + len(test.lower()) + 3
                shortened_red_log = red_results[i][2][file_name_index:] # Now this variable will have only test specific error 

                # But we can still shorten this. Since the test that failed is already in result[0], we only need line # and assertion error
                line_number_index = shortened_red_log.find(',') # Where line # ends 
                red_test_length = len(' in ' + red_results[i][0]) + 5 # + 5 for '\n   ', keeping one whitespace
                shortened_red_log = shortened_red_log[:line_number_index + 1] + shortened_red_log[line_number_index + red_test_length:]

                # Need to assign for entire tuple or else python will throw type error
                red_results[i] = (red_results[i][0], red_results[i][1], shortened_red_log)
                # Note that this for loop doesn't work when the OBC becomes unresponsive 
                # i.e. times out waiting for signal

            self.write_JSON_value(test, 'Error logs', str(red_results))
            self.write_JSON_value(test, 'Status', str(len(red_results)))
        else:
            self.change_test_button(test, 'green', 0)
            self.write_JSON_value(test, 'Error logs', '')
            self.write_JSON_value(test, 'Status', '')

            if self.run_all_tests_failures == 0: # So runAll doesn't change to green with other tests failed 
                self.change_status_button('green', 0)
            else: 
                self.change_status_button('red', self.run_all_tests_failures)

        
        self.write_JSON_value(test, 'Instance', '0')

        self.get_OBC_FW_hash()

        # Print status of test
        self.print_status_info(test)

        # Need it to update immediately
        self.DataRow.viewport().update()
        self.HILTestsTable.viewport().update()
    
    def print_status_info(self, test):
        tests_ran = self.get_JSON_member(test, 'Tests ran')
        test_has_ran = len(tests_ran) > 0 # Has this test been run before?

        if(test_has_ran): # Let's print the test's history  
            error_logs = self.get_JSON_member(test, 'Error logs')
            green_test = (len(error_logs) ==  0)# No error logs --> green 
            if(green_test):
                print(str(test) + ' test has passed! :-)')
            else: # Print error logs
                error_logs = error_logs.replace(r'\n', ' ') # Remove all new line characters
                error_logs_list = eval(error_logs) # Converting string to list by treating as literal 

                error_logs_len = len(error_logs_list) # print for all red tests
                for i in range(error_logs_len):
                    print(error_logs_list[i])
        else: 
            print('Run the test to see')

    # FW function
    @QtCore.pyqtSlot()
    def get_OBC_FW_hash(self):
        self.obc.execute(ttcqt.TTCQTRequest(
            lambda obc : obc.send_obc_cmd_str("FW_INFO", timeout=1),
            self.process_OBC_FW_info
        ))
        
    # Response from obc will only be available in this function
    def process_OBC_FW_info(self, resp: cmd_sys.OBCResponse | Exception):
        if not ttc_utils.check_cmd_sys_resp(resp, "fw_hash"):
            return
       
        # Compare to git hash
        if hex(resp.data["fw_hash"])[2:] != git_utils.get_repo_hash():
            print('WARNING: Flashed firmware hash is different than git firmware hash')
            print('Try flashing board with commit SA is running off of OR update SA with most recent flashed firmware')

    # Status buttons functions
    # Transparent (__init__) --> Yellow --> Red/Green
    # On red: button red with number of tests failed/error in button 
    @QtCore.pyqtSlot()
    def change_test_button(self, test, colour, num_of_reds):
        button = self.status_buttons[self.hil_test_classes.index(test)]
        button.setText(None) # Clear any old text 

        row_height = int(self.HILTestsTable.rowHeight(self.hil_test_classes.index(test)))

        if colour == 'red':
        # Test failed
            button.setText(str(num_of_reds))
            button.setStyleSheet("border-radius : "+str(int(row_height/2))+"; border : 1px solid black; background-color: red")

        if colour == 'yellow':
        # Test running
            button.setStyleSheet("border-radius : "+str(int(row_height/2))+"; border : 1px solid black; background-color: yellow")
        
        if colour == 'green':
        # Test passed
            button.setStyleSheet("border-radius : "+str(int(row_height/2))+"; border : 1px solid black; background-color: green")

    # change_test_button() is for individual tests, this function is reserved for run_all_tests() button
    @QtCore.pyqtSlot()
    def change_status_button(self, colour, num_of_reds):
        self.Status.setText(None) # Clear any old text
        # Tests failed
        if colour == 'red':
            self.Status.setText(str(num_of_reds))
            self.Status.setStyleSheet("border-radius : 28; border : 1px solid black; background-color: red")

        # Tests running
        if colour == 'yellow':
            self.Status.setStyleSheet("border-radius : 28; border : 1px solid black; background-color: yellow")
        
        # Tests passed
        if colour == 'green':
            self.Status.setStyleSheet("border-radius : 28; border : 1px solid black; background-color: green")
        
    # Set text functions 
    # To increase readability and not have the same 3 lines repeated like 20 times
    @QtCore.pyqtSlot()
    def set_table_text(self, row, column, Text):
        TableItem = QtWidgets.QTableWidgetItem(Text)
        TableItem.setTextAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)
        self.HILTestsTable.setItem(row, column, TableItem)

    # DataRow only has one row so row argument not necessary
    @QtCore.pyqtSlot()
    def set_data_row_text(self, column, Text):
        dataRowItem = QtWidgets.QTableWidgetItem(Text)
        dataRowItem.setTextAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)
        self.DataRow.setItem(0, column, dataRowItem)

    @QtCore.pyqtSlot()
    def handle_set_alert(self):
        pass

    # Session manager functions 
    def reset_hiltestpanel(self):
        # Reset to the defaults in DataRow 
        # 1st column, Number of HIL tests
        self.set_data_row_text(0, str(len(self.hil_test_classes)))
        # 2nd column, Number of tests
        self.set_data_row_text(1, str(self.num_HIL_tests))
        # 3rd column, Tests ran (NOT number of HIL tests, but all tests)
        self.set_data_row_text(2, str(0))
        # 4th column, Tests passed 
        self.set_data_row_text(3, str(0))
        # 5th column, Current HIL test 
        self.set_data_row_text(4, 'No HIL test')
        # Status button 
        self.Status.setText(None) # Clear any old text
        self.Status.setStyleSheet("border-radius : 28; border : 1px solid black; background-color: #00000000")

        # # Reset all DataRow history
        # self.write_JSON_value('DataRow variables', 'HIL tests', str(len(self.hil_test_classes)))
        # self.write_JSON_value('DataRow variables', 'Total tests', str(self.num_all_tests))
        # self.write_JSON_value('DataRow variables', 'Tests ran', '')
        # self.write_JSON_value('DataRow variables', 'Tests passed', '')
        # self.write_JSON_value('DataRow variables', 'Current HIL test', 'No HIL test')
        # self.write_JSON_value('DataRow variables', 'Number of reds', '')

        # Reset to the defaults in testTable
        # We also want to reset the JSON history file 
        for test in self.hil_test_classes:
            test_index = self.hil_test_classes.index(test)
            # 1) Name of test file
            # This is computed in hilTests[]
            self.set_table_text(test_index, 0, test)

            # 2) Number of tests in test file 
            # This is done in the beginning with the setup of hil_test_classes
            num_tests = str(self.test_nums[test])
            self.set_table_text(test_index, 1, num_tests)

            # 3) Total number of tests ran
            self.set_table_text(test_index, 2, str(0)) # On init it's 0 as we've ran no tests yet

            # 4) Length of time HIL test took
            self.set_table_text(test_index, 3, str(0))

            # 5) Last time test was run
            self.set_table_text(test_index, 4, "YYYY-MM-DD hh:mm:ss")

            # 6) Previous test result 
            self.set_table_text(test_index, 5, "N/A")

            # 7) Status of test
            # Set to transparent on init
            # Done manually without set_table_text() as this is a (fake) button, not text
            StatusButton = QtWidgets.QPushButton()
            row_height = int(self.HILTestsTable.rowHeight(test_index))

            StatusButton.setFixedSize(row_height, row_height) 
            StatusButton.setStyleSheet("border-radius : "+str(int(row_height/2))+"; border : 1px solid black; background-color: #00000000")

            # This container is to center the button in cell 
            StatusButtonContainer = QtWidgets.QWidget()
            StatusButtonLayout = QtWidgets.QHBoxLayout(StatusButtonContainer)
            StatusButtonLayout.addStretch() # Centers left
            StatusButtonLayout.addWidget(StatusButton)
            StatusButtonLayout.addStretch() # Centers right
            StatusButtonLayout.setContentsMargins(0, 0, 0, 0) # Centers vertically
            StatusButtonContainer.setLayout(StatusButtonLayout)

            self.HILTestsTable.setCellWidget(test_index, 6, StatusButtonContainer)

            self.status_buttons[test_index] = StatusButton

            # 8) Button to run test
            # Done manually without set_table_text() as this is a button, not text
            test_button_text = 'Run ' + test + ' test'
            TestButton = QtWidgets.QPushButton(test_button_text)
            self.HILTestsTable.setCellWidget(test_index, 7, TestButton)
            TestButton.clicked.connect(self.create_test_button(test)) 

            # # # Reset all JSON history
            # self.write_JSON_value(test, 'Number of tests', num_tests)
            # self.write_JSON_value(test, 'Tests ran', '')
            # self.write_JSON_value(test, 'Time taken (seconds)', '')
            # self.write_JSON_value(test, 'Time last ran (local)', '')
            # self.write_JSON_value(test, 'Error logs', '')
            # self.write_JSON_value(test, 'Status', '')
            # self.write_JSON_value(test, 'Instance', '')

    def write_test_info(self, history_list: dict[str, dict[str, str]], test: str):
        history_list[test] = {}  # dictionary key for test will be test name 
        for test_field in range(len(self.test_info)):
            history_list[test].update({self.test_info[test_field] : ''})

    # JSON functions 
    # create_JSON() is only used when there doesn't exist an existing history file
    def create_JSON(self):
        history_list = {}
        for test in self.hil_test_classes:
            self.write_test_info(history_list, test)

        # Lastly we need to get top row data stuff, including the status button 
        history_list['DataRow variables'] = {}
        history_list['DataRow variables'].update({'HIL tests' : str(self.num_HIL_tests)})
        history_list['DataRow variables'].update({'Total tests' : str(self.num_all_tests)})
        history_list['DataRow variables'].update({'Tests ran' : str(self.tests_ran_global)})
        history_list['DataRow variables'].update({'Tests passed' : str(self.tests_passed)})
        history_list['DataRow variables'].update({'Current HIL test' : self.current_HIL_test})
        # Button status 
        num_reds = self.tests_ran_global - self.tests_passed
        history_list['DataRow variables'].update({'Number of reds' : str(num_reds)})

        return history_list
    
    # writeJSON value will write a value to a specific JSON entry
    def write_JSON_value(self, test, info_name, text):
        # Check if log file exists 
        data: dict[str, dict[str, str]]
        if exists(str(self.JSON_path)):
            f = open(self.JSON_path)
            data = json.load(f)
            f.close()
        else: 
            # Create anew 
            data = self.create_JSON()

        if test not in data.keys():
            self.write_test_info(data, test)

        data[test][info_name] = text

        # Actually save the file
        with open(str(self.JSON_path), 'w', encoding='utf-8') as f:
            json.dump(data, f, ensure_ascii=False, indent=4)

    def get_JSON_member(self, test, info_name):
        # Check if log file exists 
        data: dict[str, dict[str, str]]
        if exists(str(self.JSON_path)):
            # Load JSON
            f = open(self.JSON_path)
            data = json.load(f)
            f.close()
        else: 
            # Create anew 
            data = self.create_JSON()

        if test not in data.keys():
            self.write_test_info(data, test)

        return data[test][info_name]
        
    # Called from sessional manager 
    def getJSONinfo(self):
        if self.existing_JSON:
            with open(str(self.JSON_path), 'r') as file:
                history_list = json.load(file)
        else:
            # create JSON file 
            history_list = self.create_JSON()

        return history_list

    def loadJSONinfo(self, hiltest_data):
        # Load DataRow variables
        # Last element of json file will be 'DataRow variables

        self.set_data_row_text(0, hiltest_data['DataRow variables']['HIL tests'])
        self.set_data_row_text(1, hiltest_data['DataRow variables']['Total tests'])
        self.set_data_row_text(2, hiltest_data['DataRow variables']['Tests ran'])
        self.set_data_row_text(3, hiltest_data['DataRow variables']['Tests passed'])
        self.set_data_row_text(4, hiltest_data['DataRow variables']['Current HIL test'])

        if(len(hiltest_data['DataRow variables']['Number of reds']) == 0):
            self.change_status_button('green', 0)
        else:
            self.change_status_button('red', hiltest_data['DataRow variables']['Number of reds'])

        # Now load table variables for each test
        for test in self.hil_test_classes:
            # Name of test and number of tests are in __init__ and reset_hiltestpanel
            # So they can be skipped as reset is called on a load session call and they're default values
            test_index = self.hil_test_classes.index(test)
            test_name = test

            # 3rd column, tests ran 
            num_tests_ran = hiltest_data[test_name]['Tests ran']
            test_has_ran = len(num_tests_ran) > 0

            if(test_has_ran):
                self.set_table_text(test_index, 2, num_tests_ran)
            else:
                self.set_table_text(test_index, 2, str(0))

            # 4th column, time taken
            time_taken = hiltest_data[test_name]['Time taken (seconds)']
            if(test_has_ran):
                self.set_table_text(test_index, 3, time_taken)
            else:
                self.set_table_text(test_index, 3, str(0))

            # 5th column, last time ran 
            last_time_ran = hiltest_data[test_name]['Time last ran (local)']
            if(test_has_ran):
                self.set_table_text(test_index, 4, last_time_ran)
            else:
                self.set_table_text(test_index, 4, "YYYY-MM-DD hh:mm:ss")
            
            # 6th column, previous run 
            error_logs = hiltest_data[test_name]['Error logs']
            num_reds = hiltest_data[test_name]['Status']

            if(test_has_ran):
                if(len(error_logs) == 0):
                    self.set_table_text(test_index, 5, "Succeeded")
                else: 
                    red_text = num_reds + ' failed'
                    self.set_table_text(test_index, 5, red_text)
            else:
                self.set_table_text(test_index, 5, "N/A")
            
            # 7th column, status button 
            current_instance = hiltest_data[test_name]['Instance']
            ran_in_instance = len(current_instance) > 0

            if(ran_in_instance):
                if (len(num_reds) > 0):
                    self.change_test_button(test_name, 'red', int(num_reds))
                else: 
                    self.change_test_button(test_name, 'green', 0)
            else:
                pass # Keep as transparent

        # Need it to update immediately
        self.DataRow.viewport().update()
        self.HILTestsTable.viewport().update()
