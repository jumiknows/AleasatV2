import unittest
from typing import Type, List, Union
import os
import time
import argparse
import sys
import threading
import pathlib
from unittest.util import safe_repr

import xmlrunner

import numpy as np

from alea.ttc.ttc import TTC
from alea.ttc.protocol.generic import routing
from alea.ttc.protocol.app import app_log
from alea.ttc.device.serial_device import get_serial_ports

PORT_ENV_VAR = "ALEA_OBC_PORT"

class HILTest(unittest.TestCase):
    PORT = os.getenv(PORT_ENV_VAR)

    # The TTC can be assigned by the code that prepares and runs the test suite in which case
    # the HILTest does not instantiate, start or stop its own TTC
    ttc : TTC = None
    logs : routing.PacketSource[app_log.OBCLog] = None

    # For internal use only
    _logs_print: routing.PacketSource[app_log.OBCLog] = None
    _logs_print_thread: threading.Thread = None
    _logs_print_stop: threading.Event = None

    _stop_ttc_after_test = False

    @staticmethod
    def create_and_start_ttc(port: str) -> TTC:
        ttc = TTC(TTC.InterfaceType.OBC_SERIAL)
        if not ttc.start(port):
            raise Exception(f"ERROR: Could not connect to {port}")
        return ttc

    @classmethod
    def setUpClass(cls):
        if cls.PORT is None:
            raise Exception("ERROR: Must set ALEA_OBC_PORT environment variable")

        if cls.ttc is None:
            cls.ttc = HILTest.create_and_start_ttc(cls.PORT)
            # Since we started the TTC we should stop it. If a TTC is provided to the class before the tests are
            # run, we won't start or stop it (the caller becomes responsible for those steps)
            cls._stop_ttc_after_test = True

        cls.logs = cls.ttc.add_log_listener(queue_size=100)

        cls._logs_print = cls.ttc.add_log_listener(queue_size=100)
        cls._logs_print_stop = threading.Event()
        cls._logs_print_thread = threading.Thread(target=cls.bg_logs_run, daemon=True)
        print("Starting log printing thread")
        cls._logs_print_thread.start()

    def setUp(self) -> None:
        # Put the satellite in a known state
        self.clear_signals() # We don't want to detect an old STARTUP_COMPLETE message
        self.ttc.reset()
        self.wait_for_signal("LOG_PRINT_GENERAL", "STARTUP_COMPLETE", timeout=3) # TODO ALEA-853 use a less ambiguous log message to indicate system is booted
        time.sleep(1) # TODO remove once ALEA-853 is implemented

    def tearDown(self) -> None:
        pass

    @classmethod
    def tearDownClass(cls) -> None:
        print("Stopping log printing thread")
        cls._logs_print_stop.set()
        cls._logs_print_thread.join(2)

        cls.ttc.remove_log_listener(cls.logs)
        cls.ttc.remove_log_listener(cls._logs_print)

        if cls._stop_ttc_after_test:
            cls.ttc.stop()

    @classmethod
    def bg_logs_run(cls):
        while not cls._logs_print_stop.is_set():
            logs = cls._logs_print.read(0.1)
            if logs:
                for log in logs:
                    print(log)

    def clear_signals(self):
        self.logs.reset()

    def wait_for_signal(self, group_name: str, pass_sig: str, fail_sig: str = None, timeout: float = None) -> app_log.OBCLog:
        start = time.time()
        while True:
            logs = self.logs.read(0.1)
            if logs:
                for log in logs:
                    if group_name == log.group_name:
                        if pass_sig == log.signal_name:
                            return log
                        elif (fail_sig != None) and (fail_sig == log.signal_name):
                            self.fail(f"Signal \"{fail_sig}\" observed: test failed.")
            if timeout is not None:
                if (time.time() - start) >= timeout:
                    self.fail("Timed-out waiting for pass signal")

    def assertEqualRelative(self, actual:float, expected:float, percent_tol:float=0.1, msg=None):
        """ Assert that two floats are relatively equal within a certain percentage difference tolerance

        Error is calculated as a percentage of expected.

        Args:
            actual: Actual value
            expected: Expected value
            percent_tol: Allowable percentage error tolerance
            msg: optional message for error

        Raises:
            failureException: Type[BaseException]
        """
        ratio = actual/expected
        percent_diff = abs(1-ratio)*100
        if percent_diff > percent_tol:
            standardMsg = f'{safe_repr(actual)} != {safe_repr(expected)} within {safe_repr(percent_tol)} percent difference'
            msg = self._formatMessage(msg, standardMsg)
            raise self.failureException(msg)

    def assertEqualRelativeVector(self, actual_vec:np.ndarray, expected_vec:np.ndarray, tolerance:float=0.001, msg=None):
        """Asserts if relative error of vectors <= tolerance. Raises error if assertion fails.

        Calculate relative error of the norm of the differece of the 
        expected vector and actual vector with the norm of the actual vector.

        Args:
            actual_vec: Actual vector
            expected_vec: Expected vector
            tolerance: Allowable error tolerance
            msg: optional message for error

        Raises:
            failureException: Type[BaseException]
        """
        error = np.linalg.norm(actual_vec - expected_vec) / np.linalg.norm(expected_vec)
        if error > tolerance:
            standardMsg = f'error ({error}) > tolerance ({tolerance}). Expected vector: {safe_repr(expected_vec)}. Actual vector: {safe_repr(actual_vec)}'
            msg = self._formatMessage(msg, standardMsg)
            raise self.failureException(msg)

def main(test_cases: Union[Type[HILTest],List[Type[HILTest]]] = None, test_dir: str | pathlib.Path = None) -> str:
    env_port = os.getenv(PORT_ENV_VAR)

    parser = argparse.ArgumentParser()
    parser.add_argument("--port", action="store", dest="port", help="OBC serial port", default=env_port)
    parser.add_argument("--list_ports", action="store_true", dest="list_ports", help="List available serial ports")
    parser.add_argument("--output_dir", action="store", dest="output_dir", help="Output directory for XML test reports")
    ns, args = parser.parse_known_args(namespace=unittest)

    # Repopulate the sys.argv with the unparsed args for unittest to handle
    sys.argv[:] = sys.argv[:1] + args

    port = ns.port

    if port is None or ns.list_ports:
        ports = get_serial_ports()
        print("Please specify a port in the ALEA_OBC_PORT environment variable or with the --port argument:")
        print("\n".join([f"    {port}" for port in ports]))
        exit(0)

    HILTest.PORT = port

    # Instantiate and assign the TTC at the top-level to save time
    HILTest.ttc = HILTest.create_and_start_ttc(HILTest.PORT)

    # Make test_cases iterable if it's not
    if test_cases is not None:
        try:
            iter(test_cases)
        except TypeError:
            test_cases = [test_cases]

        # Load test cases
        loader = unittest.TestLoader()
        loaded_cases = [loader.loadTestsFromTestCase(test_case) for test_case in test_cases]
        test_suite = unittest.TestSuite(loaded_cases)
    elif test_dir is not None:
        # Discover test cases
        loader = unittest.TestLoader()
        test_suite = loader.discover(test_dir, pattern="*_test.py")
    else:
        raise ValueError(f"main must be called with either test_cases or module")

    if ns.output_dir is not None:
        runner = xmlrunner.XMLTestRunner(output=ns.output_dir, outsuffix="")
    else:
        runner = unittest.TextTestRunner(verbosity=2)

    result = runner.run(test_suite)

    HILTest.ttc.stop()

    # Make sure exit code is set according to if tests failed or not
    sys.exit(not result.wasSuccessful())
