from pathlib import Path

import alea.test.hil.suites
from alea.test.hil import hil_test

if __name__ == "__main__":
    suites_dir = Path(alea.test.hil.suites.__file__).parent
    hil_test.main(test_dir=suites_dir)
