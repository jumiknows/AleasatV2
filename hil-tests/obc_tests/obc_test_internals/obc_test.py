import unittest
from typing import Type, List, Union
import os
import time
import argparse
import sys

import xmlrunner

from obcpy.obc_handler import OBCHandler
import obcpy.utils

PORT_ENV_VAR = "ALEA_OBC_PORT"

class OBCTest(unittest.TestCase):
    PORT = os.getenv(PORT_ENV_VAR)

    obc : OBCHandler = None

    @classmethod
    def setUpClass(cls):
        if cls.PORT is None:
            raise Exception("ERROR: Must set ALEA_OBC_PORT environment variable")

        cls.obc = OBCHandler()
        if not cls.obc.start(cls.PORT):
            raise Exception(f"ERROR: Could not connect to {cls.PORT}")

    def setUp(self) -> None:
        # Put the OBC in a known state
        self.obc.reset()
        self.obc.reset() # TODO remove after serial protocol is updated to have sync bytes
        time.sleep(2)

    def tearDown(self) -> None:
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
        ports = obcpy.utils.get_serial_ports()
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
