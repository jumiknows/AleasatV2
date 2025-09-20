import unittest

import numpy as np

from alea.sim.kernel.root_models import AleasimRootModel
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.spacecraft.spacecraft import Spacecraft
from alea.sim.spacecraft.aocs import AOCSModel
from alea.sim.spacecraft.eps.power_system import PowerSystemModel
from alea.sim.spacecraft.spacecraft import Spacecraft
from parameterized import parameterized
from alea.sim.math_lib import Quaternion

class AOCSTest(unittest.TestCase):
    
    def setUp(self):
        self.kernel = AleasimKernel(dt=0.1, date=2024.2)
        root_model = AleasimRootModel(self.kernel)
        self.kernel.add_model(root_model)

        self.adyn: AttitudeDynamicsModel = self.kernel.get_model(AttitudeDynamicsModel)
        self.odyn: OrbitDynamicsModel = self.kernel.get_model(OrbitDynamicsModel)
        self.sc: Spacecraft = self.kernel.get_model(Spacecraft)
        self.aocs = self.sc.aocs
        self.power_sys: PowerSystemModel = self.kernel.get_model(PowerSystemModel)
    
    def tearDown(self):
        del self.adyn
        del self.odyn
        del self.sc
        del self.aocs
        del self.power_sys

        self.kernel.kill()
        del self.kernel

    @unittest.skip("Skipped in pipeline because it takes 4+ mins.")
    def test_aocs_detumbling(self):
        """Test AOCS detumbling"""

        self.adyn._state = np.array([1.0, 0.0, 0.0, 0.0, 4.0, 0.5, 1.0])
        
        np.testing.assert_almost_equal(self.adyn._state, np.array([1.0, 0.0, 0.0, 0.0, 4.0, 0.5, 1.0]))
        init_v = self.adyn.angular_velocity.copy()
        init_t = self.kernel.time

        self.aocs.set_mode(AOCSModel.AOCSMode.DETUMBLE_MAG)
        
        self.kernel.advance(100)
        self.assertEqual(self.aocs._mode, AOCSModel.AOCSMode.DETUMBLE_MAG)

        self.kernel.advance(6000)
        final_v = self.adyn.angular_velocity
        final_t = self.kernel.time
        
        print(f"AOCS detumbled from {init_v} [rad/s] to {final_v} [rad/s] in {final_t-init_t:.3f} seconds.")
        self.assertLess(np.linalg.norm(self.adyn.angular_velocity), np.deg2rad(5), "Final angular velocity is not less than 5 deg/s!")

    @parameterized.expand([
        [Quaternion([0.44079742, 0.58408327, 0.54548583, 0.40864358])],
        [Quaternion([0.18180749, 0.82954125, 0.42537837, 0.31282678])],
    ])
    def test_aocs_pointing_error(self, q_target: Quaternion):
        """Test AOCS pointing error converges to < 2 degrees within 5 minutes"""

        self.adyn._state[0:4] = Quaternion.identity().to_array()

        self.aocs.set_mode(AOCSModel.AOCSMode.POINTING_QUAT, q=q_target)

        self.assertEqual(self.aocs._mode, AOCSModel.AOCSMode.POINTING_QUAT)

        self.kernel.advance(60.0*2)

        self.assertLessEqual(self.aocs.abs_pointing_error, 2.0, "Pointing error is not less than 2 degrees!")

if __name__ == '__main__':
    unittest.main()
