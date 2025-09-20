import unittest
import numpy as np
from parameterized import parameterized
from alea.sim.epa.disturbance_model import DisturbanceModel, DisturbanceModelConfig, calculate_atmospheric_density
from alea.sim.math_lib import Quaternion, cross
from alea.sim.kernel.kernel import AleasimKernel
class DisturbanceModelTest(unittest.TestCase):
    """
    Unit tests for the DisturbanceModel.
    
    These tests call the functions directly by injecting known inputs via the optional parameters.
    This decouples the tests from the external models.
    
    References:
      - Markley & Crassidis, "Fundamentals of Spacecraft Attitude Determination and Control", 2014.
    """
    def setUp(self):
        # Create a minimal kernel (we don't rely on full models in these tests)
        self.kernel = AleasimKernel(date=2020.0)

        # Create a config with typical CubeSat values.
        #    * Effective drag/SRP area ~ 0.02–0.03 m²,
        #    * CP-COM offsets (lever arms) ~ 1–2 cm,
        #    * Residual magnetic dipole on the order of 1e-3 A·m².
        cfg_dict = {
            "residual_mag_dipole": [0.001, 0, 0],
            "drag_coefficient": 2.2,
            "drag_effective_area": 0.03,
            "drag_lever_arm": [0.02, 0, 0],
            "srp_coefficient": 1.3,
            "srp_effective_area": 0.03,
            "srp_lever_arm": [0, 0.01, 0],
            "earth_gravitational_parameter": 3.986004418e14,
            "exp_atmoshere_h0": 450e3, 
            "exp_atmosphere_rho0": 1.585e-12,
            "exp_atmosphere_H": 62.2e3
        }
        self.cfg = DisturbanceModelConfig(**cfg_dict)
        self.dist_model = DisturbanceModel(self.kernel, cfg=self.cfg)

    @parameterized.expand([
        # Case 1: Satellite at [R,0,0] (R = 6771 km) with nadir = [-1,0,0] and identity inertia.
        ([6771e3, 0, 0], np.array([-1, 0, 0]), np.zeros(3)),
        # Case 2: Satellite at [6771e3, 100e3, 0] (non-axis aligned) but assume nadir is provided as [-0.98, -0.2, 0],
        # and with identity inertia so that cross(nadir, nadir)=0.
        ([6771e3, 100e3, 0], np.array([-0.98, -0.2, 0]), np.zeros(3)),
        # Case 3: Satellite at [7000e3, 0, 0] with nadir = [-1,0,0] yields zero torque if I is identity.
        ([7000e3, 0, 0], np.array([-1, 0, 0]), np.zeros(3))
    ])
    def test_gravitational_torque(self, r_eci_list, nadir_body, expected):
        r_eci = np.array(r_eci_list)
        # Override inertia matrix to identity for simplicity.
        self.dist_model.adyn = type("DummyAD", (), {})()
        self.dist_model.adyn.cfg = type("DummyCfg", (), {})()
        self.dist_model.adyn.cfg.J = np.eye(3)

        torque = self.dist_model.gravitational_torque(r_eci=r_eci, nadir_body=nadir_body)
        np.testing.assert_array_almost_equal(torque, expected, decimal=8)

    @parameterized.expand([
        # Residual dipole: [0.001,0,0], B_field: [0, 0, 50000] nT => expected torque: cross([0.001,0,0], [0,0,5e-5]) = [0, -5e-8, 0]
        ([0.001, 0, 0], [0, 0, 50000], [0, -5e-8, 0]),
        # Residual dipole: [0, 0.001, 0], B_field: [50000, 0, 0] nT => expected: [0, 0, -5e-8]
        ([0, 0.001, 0], [50000, 0, 0], [0, 0, -5e-8]),
        # Residual dipole: [0, 0, 0.001], B_field: [0, 50000, 0] nT => expected: cross([0,0,0.001], [0,5e-5,0]) = [-5e-8, 0, 0]
        ([0, 0, 0.001], [0, 50000, 0], [-5e-8, 0, 0])
    ])
    def test_magnetic_residual_torque(self, residual_dipole, B_body_nT, expected):
        self.cfg.residual_mag_dipole = np.array(residual_dipole)
        torque = self.dist_model.magnetic_residual_torque(B_body_nT=np.array(B_body_nT))
        np.testing.assert_array_almost_equal(torque, np.array(expected), decimal=10)

    @parameterized.expand([
        # Case 1: Altitude 400 km, rel_vel along y-axis [0,7500,0]
        (400e3, [0, 7500, 0]),
        # Case 2: Altitude 300 km, rel_vel along x-axis [7500, 0, 0]
        (300e3, [7500, 0, 0]),
        # Case 3: Altitude 500 km, rel_vel along z-axis [0, 0, 7500]
        (500e3, [0, 0, 7500])
    ])
    def test_aerodynamic_drag_torque(self, altitude_m, rel_vel_list):
        rel_vel_body = np.array(rel_vel_list)
        density = calculate_atmospheric_density(altitude_m, self.cfg.exp_atmoshere_h0, self.cfg.exp_atmosphere_rho0, self.cfg.exp_atmosphere_H)
        v_mag = np.linalg.norm(rel_vel_body)
        F_drag = 0.5 * density * (v_mag**2) * self.cfg.drag_coefficient * self.cfg.drag_effective_area
        drag_force_body = -F_drag * (rel_vel_body / v_mag)
        expected_torque = cross(self.cfg.drag_lever_arm, drag_force_body)
        torque = self.dist_model.aerodynamic_drag_torque(altitude_m=altitude_m, rel_vel_body=rel_vel_body)
        np.testing.assert_array_almost_equal(torque, expected_torque, decimal=10)

    @parameterized.expand([
        # Case 1: sun_dir = [1, 0, 0]
        ( [1, 0, 0],
          # Expected: F_srp = 4.5e-6 * 0.03 * 1.3, torque = cross([0, 0.01, 0], F_srp*[1,0,0]) = [0, 0, -0.01*F_srp]
          [0, 0, -0.01 * (4.5e-6 * 0.03 * 1.3)] ),
        # Case 2: sun_dir = [0, 1, 0] (parallel to lever arm) => expected torque zero.
        ( [0, 1, 0],
          [0, 0, 0] ),
        # Case 3: sun_dir = [0, 0, 1] => expected: torque = cross([0, 0.01, 0], [0, 0, F_srp]) = [0.01*F_srp, 0, 0]
        ( [0, 0, 1],
          [0.01 * (4.5e-6 * 0.03 * 1.3), 0, 0] )
    ])
    def test_solar_radiation_pressure_torque(self, sun_dir_list, expected):
        sun_dir = np.array(sun_dir_list)
        # Compute expected force magnitude:
        expected = np.array(expected)
        torque = self.dist_model.solar_radiation_pressure_torque(sun_dir=sun_dir)
        np.testing.assert_array_almost_equal(torque, expected, decimal=10)

if __name__ == '__main__':
    unittest.main()
