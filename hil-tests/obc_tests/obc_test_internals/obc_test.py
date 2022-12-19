import unittest
from typing import Type, List, Union
import os
import time
import argparse
import sys

import xmlrunner

from obcpy.protocol.routing import PacketSource
from obcpy.obc_protocol.log import OBCLog
from obcpy.obc_handler import OBCHandler
from  obcpy.utils.serial import get_serial_ports

PORT_ENV_VAR = "ALEA_OBC_PORT"

class OBCTest(unittest.TestCase):
    PORT = os.getenv(PORT_ENV_VAR)

    obc : OBCHandler = None
    logs : PacketSource[OBCLog] = None

    @classmethod
    def setUpClass(cls):
        if cls.PORT is None:
            raise Exception("ERROR: Must set ALEA_OBC_PORT environment variable")

        cls.obc = OBCHandler()
        if not cls.obc.start(cls.PORT):
            raise Exception(f"ERROR: Could not connect to {cls.PORT}")

        cls.logs = cls.obc.interface.protocol.add_log_dest()

    def setUp(self) -> None:
        # Put the OBC in a known state
        self.obc.reset()
        time.sleep(2)

    def tearDown(self) -> None:
        pass

    def wait_for_keyword(self, pass_key: str, fail_key: str = None) -> OBCLog:
        while 1:
            logs = self.logs.read()
            if logs:
                for log in logs:
                    print(log)
                    try:
                        log_text = log.payload.decode("ascii")
                        if pass_key in log_text:
                            return log
                        elif (fail_key is not None) and (fail_key in log_text):
                            self.fail(f"\"{fail_key}\" observed: test failed.")
                    except UnicodeDecodeError:
                        pass

    @classmethod
    def tearDownClass(cls) -> None:
        cls.obc.stop()

def main(test_cases: Union[Type[OBCTest],List[Type[OBCTest]]]) -> str:
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

    # Make test_cases iterable if it's not
    try:
        iter(test_cases)
    except TypeError:
        test_cases = [test_cases]

    # Assign the ports
    for test_case in test_cases:
        test_case.PORT = port

    if ns.output_dir is not None:
        runner = xmlrunner.XMLTestRunner(output=ns.output_dir, outsuffix="")
    else:
        runner = unittest.TextTestRunner(verbosity=2)

    # Instantiate test cases
    test_suites = [unittest.makeSuite(test_case) for test_case in test_cases]
    all_tests = unittest.TestSuite(test_suites)

    result = runner.run(all_tests)

    # Make sure exit code is set according to if tests failed or not
    sys.exit(not result.wasSuccessful())
