import unittest
import sys

from compute.precomputed_test import PreComputedTest

from epa.attitude_test import AttitudeDynamicsTest
from epa.orbit_dynamics_test import OrbitDynamicsTest
from epa.earth_magnetic_field_test import EarthMagneticFieldTest
from epa.frame_conversions_test import FrameConversionsTest

from kernel.kernel_test import KernelTest
from kernel.frames_test import FramesTest
from kernel.scheduler_test import SchedulerTest
from kernel.time_cached_test import TimeCachedTest

from algorithms.wahba_test import WahbaTest

from spacecraft.eps_test import EpsTest, SolarPanelsTest
from spacecraft.simple_sensors_test import SimpleSensorsTest
from spacecraft.simple_mag_test import SimpleMagTest
from spacecraft.reaction_wheel_test import ReactionWheelTest
from spacecraft.mtq_test import MagnetorquerTest
from spacecraft.spacecraft_test import SpacecraftTest

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
        MagnetorquerTest,
        EpsTest,
        SolarPanelsTest,
        SimpleSensorsTest,
        ReactionWheelTest,
        SimpleMagTest,
        SpacecraftTest
    ]

    runner = unittest.TextTestRunner(verbosity=2)

    test_suites = [unittest.makeSuite(test_case) for test_case in cases]
    all_tests = unittest.TestSuite(test_suites)

    result = runner.run(all_tests)

    # Make sure exit code is set according to if tests failed or not
    sys.exit(not result.wasSuccessful())
