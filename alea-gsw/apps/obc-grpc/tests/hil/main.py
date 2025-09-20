from alea.test.hil import suites
from alea.test.hil.hil_test import HILTest
from alea.obc_grpc.ttc import TTC_GRPC
from pathlib import Path
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


def create_and_start_ttc(port: str) -> TTC_GRPC:
    ttc = TTC_GRPC()
    ttc.start(port)
    print("Waiting for TTC to start...")
    return ttc


def main(
    test_cases: Union[Type[HILTest], List[Type[HILTest]]] = None,
    test_dir: str | pathlib.Path = None,
) -> str:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output_dir",
        action="store",
        dest="output_dir",
        help="Output directory for XML test reports",
    )
    ns, args = parser.parse_known_args(namespace=unittest)

    # Repopulate the sys.argv with the unparsed args for unittest to handle
    sys.argv[:] = sys.argv[:1] + args

    port = os.getenv("ALEA_OBC_PORT")

    if os.getenv("ALEA_OBC_PORT") is None:
        print("Please specify a port in the ALEA_OBC_PORT environment variable:")
        exit(0)

    HILTest.PORT = port

    # Instantiate and assign the TTC at the top-level to save time
    HILTest.ttc = create_and_start_ttc(HILTest.PORT)

    resp = HILTest.ttc.send_obc_cmd("I2C_RESET")
    if not resp.is_success:
        print("Failed to reset I2C bus before starting HIL tests")
        sys.exit(1)

    # Make test_cases iterable if it's not
    if test_cases is not None:
        try:
            iter(test_cases)
        except TypeError:
            test_cases = [test_cases]

        # Load test cases
        loader = unittest.TestLoader()
        loaded_cases = [
            loader.loadTestsFromTestCase(test_case) for test_case in test_cases
        ]
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

    # Make sure exit code is set according to if tests failed or not
    sys.exit(not result.wasSuccessful())


if __name__ == "__main__":
    suites_dir = Path(suites.__file__).parent
    main(test_dir=suites_dir)
