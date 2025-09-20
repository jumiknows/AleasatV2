import unittest
import numpy as np
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.math_lib.shapes import WGS84Earth
from alea.sim.epa.earth_albedo_model import EarthAlbedoModel, EarthAlbedoConfig

class EarthAlbedoModelTest(unittest.TestCase):

    def setUp(self):
        """Set up the test environment"""
        self.kernel = AleasimKernel()
        self.config = EarthAlbedoConfig()
        self.model = EarthAlbedoModel(self.kernel, cfg=self.config, name="")
    
    def test_initialization(self):
        """Test if the EarthAlbedoModel initializes correctly"""
        self.assertEqual(self.model._f_sun, 1358)
        self.assertEqual(self.model._albedo, 0.3)
        self.assertIsInstance(self.model.earth, WGS84Earth)
    
    def test_get_area_current(self):
        """Test the _getAreaCurrent method with mock inputs"""
        i_max = 0.5
        max_css_fov_deg = 30
        n_e_orbital = np.array([0, 0, 1])
        sun_vector_eci = np.array([1, 0, 0])
        n_css_body = np.array([0, 1, 0])
        u_css_orbital = np.array([0, 0, 1])

        di, d_flux = self.model._getAreaCurrent(
            i_max, max_css_fov_deg, n_e_orbital, sun_vector_eci, n_css_body, u_css_orbital
        )
        
        self.assertIsInstance(di, float)
        self.assertIsInstance(d_flux, float)

        self.assertEqual(di, 0.0)
        self.assertEqual(d_flux, 0.0)
    
    def test_get_total_current(self):
        """Test getTotalCurrent method with mock inputs"""
        i_max = 0.5
        max_css_fov_deg = 30
        sun_vector_eci = np.array([1, 0, 0])
        n_css_body = np.array([0, 1, 0])

        reflectance_current, reflectance_flux = self.model.getTotalCurrent(
            i_max, max_css_fov_deg, sun_vector_eci, n_css_body
        )
        
        self.assertIsInstance(reflectance_current, float)
        self.assertIsInstance(reflectance_flux, float)

if __name__ == '__main__':
    unittest.main()
