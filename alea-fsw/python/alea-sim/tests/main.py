import unittest
import sys
from pathlib import Path
import argparse

import xmlrunner

def main(test_dir: str | Path):
    parser = argparse.ArgumentParser()
    parser.add_argument("--output_dir", action="store", dest="output_dir", help="Output directory for XML test reports")
    ns, args = parser.parse_known_args(namespace=unittest)

    loader = unittest.TestLoader()
    test_suite = loader.discover(test_dir, pattern="*_test.py")

    if ns.output_dir is not None:
        runner = xmlrunner.XMLTestRunner(output=ns.output_dir, outsuffix="")
    else:
        runner = unittest.TextTestRunner(verbosity=2)

    result = runner.run(test_suite)

    # Make sure exit code is set according to if tests failed or not
    sys.exit(not result.wasSuccessful())

if __name__ == "__main__":
    suites_dir = Path(__file__).parent
    main(test_dir=suites_dir)
