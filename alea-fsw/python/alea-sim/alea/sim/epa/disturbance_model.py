import numpy as np
from dataclasses import dataclass
from pathlib import Path

from alea.sim.configuration import Configurable
from alea.sim.kernel.kernel import AleasimKernel, SharedMemoryModelInterface
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.math_lib import cross, Quaternion
from alea.sim.epa.atmospheric_density import calculate_atmospheric_density
from alea.sim.epa.frame_conversions import OMEGA_ECEF_ECI, OMEGA_EARTH

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
@dataclass
class DisturbanceModelConfig:
    """
    Configuration for the DisturbanceModel.
    
    References & Rationale:
      - Markley & Crassidis, "Fundamentals of Spacecraft Attitude Determination and Control"
      - Typical 1U CubeSat geometry: 10 cm x 10 cm x 10 cm => 0.01 m^2 face area
      - Often pick ~ 0.02 - 0.03 m^2 for drag & SRP "effective" area to account for edges, 
        deployables, or off-nominal attitudes.
      - CP (Centre of Pressure) - COM (Centre of Mass) offsets of ~1 - 2 cm are common for small satellites with slight mass/geometry 
        asymmetries.
    """

    # Magnetic dipole (A·m^2)
    residual_mag_dipole: np.ndarray
    
    # Atmosphere parameters
    exp_atmoshere_h0: float
    exp_atmosphere_rho0: float
    exp_atmosphere_H: float

    # Drag parameters
    drag_coefficient: float
    drag_effective_area: float
    drag_lever_arm: np.ndarray

    # SRP parameters
    srp_coefficient: float
    srp_effective_area: float
    srp_lever_arm: np.ndarray
    
    solar_radiation_pressure: float = 4.5e-6 # [N/m^2]
    earth_gravitational_parameter: float = 3.986004418e14 # [m^3/s^2]

    def __post_init__(self):
        if not isinstance(self.residual_mag_dipole, np.ndarray):
            self.residual_mag_dipole = np.array(self.residual_mag_dipole)
        if not isinstance(self.drag_lever_arm, np.ndarray):
            self.drag_lever_arm = np.array(self.drag_lever_arm)
        if not isinstance(self.srp_lever_arm, np.ndarray):
            self.srp_lever_arm = np.array(self.srp_lever_arm)

class DisturbanceModel(Configurable[DisturbanceModelConfig], SharedMemoryModelInterface, AbstractModel):
    """
    Disturbance model for computing external torques on a CubeSat.
    
    This includes:
      - Gravity gradient torque
      - Residual magnetic dipole torque
      - Aerodynamic drag torque
      - Solar radiation pressure torque
    
    References:
      - F. Landis Markley & J.L. Crassidis, "Fundamentals of Spacecraft Attitude Determination 
        and Control", 2014.
    """

    def __init__(self, sim_kernel: AleasimKernel, cfg: str | Path | dict | DisturbanceModelConfig = "disturbance_model") -> None:
        super().__init__(name='disturbance_model', sim_kernel=sim_kernel, cfg=cfg, cfg_cls=DisturbanceModelConfig)

    def connect(self):
        self.adyn: AttitudeDynamicsModel = self.kernel.get_model('attitude_dynamics') #find by string due to circular import issue
        self.odyn: OrbitDynamicsModel = self.kernel.get_model(OrbitDynamicsModel)
        self.mag_model: EarthMagneticFieldModel = self.kernel.get_model(EarthMagneticFieldModel)

    def get_T_disturbance(self) -> np.ndarray:
        """
        Return the total disturbance torque [tx, ty, tz] in the BODY frame, combining:
          - gravity gradient
          - magnetic dipole
          - aerodynamic drag
          - solar radiation pressure (SRP)
        """
        t_gg = self.gravitational_torque()
        t_mag = self.magnetic_residual_torque()
        t_aero = self.aerodynamic_drag_torque()
        t_srp = self.solar_radiation_pressure_torque()
        t_total = t_gg + t_mag + t_aero + t_srp
        self.save_chunk_to_memory(np.hstack([t_gg, t_mag, t_aero, t_srp]))
        return t_total

    def gravitational_torque(self, r_eci: np.ndarray = None, nadir_body: np.ndarray = None) -> np.ndarray:
        """
        Compute the gravity gradient torque in the BODY frame.
        
        Gravity gradient torque T_gg = 3μ / r^3 * [ r_hat × (I * r_hat ) ]
        Reference:
          Markley & Crassidis, eqn 3.155 (approx.)
        """
        if r_eci is None:
          r_eci = self.odyn.position_eci
        r_mag = np.linalg.norm(r_eci)

        if nadir_body is None:
          # Convert from ECI to body frame to get the 'nadir' direction
          # Note: If you define 'nadir' as from spacecraft -> Earth, it's -r_eci
          nadir_body = self.kernel.body_frame.transform_vector_from_frame(-r_eci / r_mag, self.kernel.eci_frame)

        tau_gg = (
            3.0 * self.cfg.earth_gravitational_parameter / (r_mag**3) *
            cross(nadir_body, self.adyn.cfg.J @ nadir_body)
        )
        return tau_gg
    
    def magnetic_residual_torque(self, B_body_nT: np.ndarray = None) -> np.ndarray:
        """
        Calculate torque from the spacecraft's residual magnetic dipole crossing the local B-field.
        
        T_mag = m x B
        """
        if B_body_nT is None:
          B_body_nT = self.mag_model.mag_field_vector_body
        # B is given in nT by EarthMagneticFieldModel, so we convert to Tesla.
        B_body_T = B_body_nT * 1e-9
        return cross(self.cfg.residual_mag_dipole, B_body_T)

    def aerodynamic_drag_torque(self, altitude_m: float = None, rel_vel_body: np.ndarray = None) -> np.ndarray:
        """
        Compute the aerodynamic drag torque.
        
        Drag force: F_d = 0.5 * ρ * v² * Cd * A_eff
        Torque: T_drag = lever_arm × F_d

        Reference: Markley & Crassidis
        """
        if altitude_m is None:
          altitude_m = self.odyn.position_lla[2]

        #atmopsheric density at spacecraft altitude
        density = calculate_atmospheric_density(altitude_m, self.cfg.exp_atmoshere_h0, self.cfg.exp_atmosphere_rho0, self.cfg.exp_atmosphere_H)

        if rel_vel_body is None:
          # here we obtain the velocity of the spacecraft relative to the atmosphsere in the body frame
          # this is roughly achieved by subtracting the tangential velocity of the astmosphere at that altitude due to Earth's rotation rate (OMEGA_EARTH)
          # and finally accounting for expressing this velocity in the body coordinate frame
          # eqn 3.161
          rel_vel_body_no_rot = self.odyn.velocity_eci
          rel_vel_body_no_rot[0] += OMEGA_EARTH * self.odyn.position_eci[1]
          rel_vel_body_no_rot[1] -= OMEGA_EARTH * self.odyn.position_eci[0]
          rel_vel_body = self.kernel.body_frame.transform_vector_from_frame(rel_vel_body_no_rot, self.kernel.eci_frame)

        v_mag = np.linalg.norm(rel_vel_body)
        F_drag = 0.5 * density * (v_mag**2) * self.cfg.drag_coefficient * self.cfg.drag_effective_area
        drag_force_body = -F_drag * (rel_vel_body / v_mag)
        return cross(self.cfg.drag_lever_arm, drag_force_body) #aerodynamic torque

    def solar_radiation_pressure_torque(self, sun_dir: np.ndarray = None) -> np.ndarray:
        """
        Compute the solar radiation pressure (SRP) torque.
        
        SRP force: F_srp = P_s * A_eff * CR
        Torque: T_srp = lever_arm × F_srp

        Reference: Markley & Crassidis
        """

        if sun_dir is None:
          # SRP is 0 if spacecraft is not in sunlight
          if not self.odyn.is_sunlit:
              return np.zeros(3)

          sun_eci_norm = self.odyn.sun_vector_norm

          sun_body = self.kernel.body_frame.transform_vector_from_frame(sun_eci_norm, self.kernel.eci_frame)
          sun_dir = sun_body / np.linalg.norm(sun_body)

        F_srp = self.cfg.solar_radiation_pressure * self.cfg.srp_effective_area * self.cfg.srp_coefficient
        srp_force = F_srp * sun_dir
        return cross(self.cfg.srp_lever_arm, srp_force) #srp torque

    @property
    def saved_state_element_names(self):
        if not hasattr(self, '_saved_state_element_names'):
            self._saved_state_element_names = []
            for prefix in ['t_gg', 't_mag', 't_aero', 't_srp']:
                self._saved_state_element_names.extend([f'{prefix}_{ax}' for ax in ('x', 'y', 'z')])
        return self._saved_state_element_names

    @property
    def saved_state_size(self):
        return len(self.saved_state_element_names)
