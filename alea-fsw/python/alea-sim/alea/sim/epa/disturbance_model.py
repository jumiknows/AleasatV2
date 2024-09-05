import numpy as np

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.kernel.generic.abstract_model import AbstractModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
import math 

class DisturbanceModel(AbstractModel):

    default_name = 'disturbance_model'
    def __init__(self, sim_kernel: AleasimKernel) -> None:
        super().__init__(self.default_name, sim_kernel)
        self.configure()
        self.logger.info("initialized")

    def configure(self):
        cfg_disturbance = self.get_config()
        
        cfg_attitude = self.get_config('attitude_dynamics')

        self.params['J'] = cfg_attitude['J']
        self.params['Cm'] = cfg_attitude['Cm']

        
        self._integrator_type = cfg_disturbance['default_integrator']
        self.logger.info(f'loaded intertia matrix cfg {cfg_attitude["J"]}')

        ## Gravity gradient 

        gravitygradient_cfg:dict = cfg_disturbance['gravity_gradient_elements']
        G = gravitygradient_cfg['G']
        M = gravitygradient_cfg['M']
        Rs = gravitygradient_cfg['Rs']
        Re = gravitygradient_cfg['Re']
        self.Radius = Re + Rs

        self.w_o = np.sqrt(G*M/math.pow(self.Radius,3))

        ## Solar pressure
        solar_pressure_cfg:dict = cfg_disturbance['solar_pressure_elements']
        self.reflectance_factor = solar_pressure_cfg['reflectance_factor']
        self.Fs = solar_pressure_cfg['Fs']
        self.c = solar_pressure_cfg['c']


        ## Megnetic residual

        magnetic_residual_cfg:dict = cfg_disturbance['magnetic_residual_elements']
        self.mean_rm_base = magnetic_residual_cfg['mean_rm_base']
        self.maximum_area = magnetic_residual_cfg['maximum_area']

    def connect(self):
        self.odyn: OrbitDynamicsModel = self.kernel.get_model(OrbitDynamicsModel)
        self.mag_model: EarthMagneticFieldModel = self.kernel.get_model(EarthMagneticFieldModel)

    @property
    def config_name(self) -> str:
        return 'disturbance_model'
    
    
    def get_T_disturbance(self,q) -> np.ndarray:
        sun_vector = (self.odyn.sun_vector_norm).T
        #body to orbit rot mat
        R_BO = self.kernel.orbit_frame.get_transformation_to(self.kernel.body_frame).rotation
        return self.gravitational_torque(R_BO) + self.solar_pressure(R_BO,sun_vector) + self.magnetic_residual_torque()
    
    def gravitational_torque(self,R_BO) -> np.ndarray:
        J = self.params.get('J') #inertia matrix
        nadir = R_BO[:,0]
        tau_g = 3*math.pow(self.w_o,2)*np.cross(nadir,J@nadir)
        return tau_g

    def solar_pressure(self,R_BO, sun_vector) -> np.ndarray:
        Cm = self.params.get('Cm') #inertia matrix
        Cm = np.array(Cm).T
        R_OB = R_BO.T
        y_0 = -R_OB @ sun_vector #% Sun vector in body frame
        y_0 = y_0 / np.linalg.norm(y_0)


        proj_Xb_Zo = np.dot([1, 0, 0], y_0) * y_0
        proj_Yb_Zo = np.dot([0, 1, 0], y_0) * y_0
        proj_Zb_Zo = np.dot([0, 0, 1], y_0) * y_0

        proj_Xb_XYo = np.array([1, 0, 0]) - proj_Xb_Zo  # Projection of each body frame axis unit vector to orbit frame x,y plane
        proj_Yb_XYo = np.array([0, 1, 0]) - proj_Yb_Zo
        proj_Zb_XYo = np.array([0, 0, 1]) - proj_Zb_Zo

        Ax = 0.034 * np.linalg.norm(np.cross(proj_Yb_XYo, proj_Zb_XYo))  # Surface projections to orbit frame x,y plane
        Ay = 0.034 * np.linalg.norm(np.cross(proj_Xb_XYo, proj_Zb_XYo))
        Az = 0.01 * np.linalg.norm(np.cross(proj_Xb_XYo, proj_Yb_XYo))
        self.Area = np.array([Ax, Ay, Az])

        cos_Xb_Xo = np.dot([1, 0, 0], y_0)
        cos_Yb_Yo = np.dot([0, 1, 0], y_0)
        cos_Zb_Zo = np.dot([0, 0, 1], y_0)
        self.cosines = np.array([cos_Xb_Xo, cos_Yb_Yo, cos_Zb_Zo])

        solar_pressure_center = np.diag([0.05, 0.05, 0.17])

        solar_pressure_center[0, :] *= np.sign(-cos_Xb_Xo)
        solar_pressure_center[1, :] *= np.sign(-cos_Yb_Yo)
        solar_pressure_center[2, :] *= np.sign(-cos_Zb_Zo)

        T1 = (self.Fs / self.c) * Ax * (1 + self.reflectance_factor) * np.cross(y_0, solar_pressure_center[0, :] - Cm)
        T2 = (self.Fs / self.c) * Ay * (1 + self.reflectance_factor) * np.cross(y_0, solar_pressure_center[1, :] - Cm)
        T3 = (self.Fs / self.c) * Az * (1 + self.reflectance_factor) * np.cross(y_0, solar_pressure_center[2, :] - Cm)


        tau_sp = T1 + T2 + T3
        return tau_sp
    
    def magnetic_residual_torque(self) -> np.ndarray:

        """
        Calculate the residual magnetic torque acting on the spacecraft.

        Parameters:
        Area (numpy array): Satellite's area projected to the sun
        cosines (numpy array): Cosine of angle between Body frame axes and orbit frame z-axis
        B_body (numpy array): Magnetic field in the body frame

        Returns:
        tau_rm (numpy array): Residual magnetic torque
        rm (numpy array): Residual magnetic moment
        """
            
        B_body = self.mag_model.mag_field_vector_body*1E-9

        sign_vector = np.sign(-self.cosines)
        self.rm = self.mean_rm_base + (0.005 * (self.Area / self.maximum_area) * sign_vector) + 0.0005 * np.random.rand(3)
        tau_rm = np.cross(self.rm, B_body)
        
        return tau_rm
