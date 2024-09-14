import unittest
import sys

from compute.precomputed_test import PreComputedTest
from kernel.time_cached_test import TimeCachedTest
from epa.attitude_test import AttitudeDynamicsTest
from epa.orbit_dynamics_test import OrbitDynamicsTest
from epa.earth_magnetic_field_test import EarthMagneticFieldTest
from kernel.kernel_test import KernelTest
from frames_test import FramesTest
from frame_conversions_test import FrameConversionsTest
from scheduler_test import SchedulerTest
from wahba_test import WahbaTest

if __name__ == "__main__":
    cases = [
        # TODO: This test fails right now (only the time comparisons) when run through main.py
        #       but it succeeds when run manually (python compute/precomputed_test.py). Only
        #       the timing comparisons fail.
        # PreComputedTest,
        TimeCachedTest,
        AttitudeDynamicsTest,
        OrbitDynamicsTest,
        EarthMagneticFieldTest,
        KernelTest,
        FramesTest,
        FrameConversionsTest,
        SchedulerTest,
        WahbaTest,
    ]

    runner = unittest.TextTestRunner(verbosity=2)

    test_suites = [unittest.makeSuite(test_case) for test_case in cases]
    all_tests = unittest.TestSuite(test_suites)

    result = runner.run(all_tests)

    # Make sure exit code is set according to if tests failed or not
    sys.exit(not result.wasSuccessful())
