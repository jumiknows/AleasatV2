import unittest
import sys

from atttitude_test import AttitudeDynamicsTest
from environment_test import MagneticEnvironmentTest
from orbit_test import OrbitDynamicsTest
from kernel_test import KernelTest
from frames_test import FramesTest
from frame_conversions_test import FrameConversionsTest
from scheduler_test import SchedulerTest
from wahba_test import WahbaTest

if __name__ == "__main__":
    cases = [
        AttitudeDynamicsTest,
        MagneticEnvironmentTest,
        OrbitDynamicsTest,
        KernelTest,
        FramesTest,
        FrameConversionsTest,
        SchedulerTest,
        WahbaTest
    ]

    runner = unittest.TextTestRunner(verbosity=2)

    test_suites = [unittest.makeSuite(test_case) for test_case in cases]
    all_tests = unittest.TestSuite(test_suites)

    result = runner.run(all_tests)

    # Make sure exit code is set according to if tests failed or not
    sys.exit(not result.wasSuccessful())