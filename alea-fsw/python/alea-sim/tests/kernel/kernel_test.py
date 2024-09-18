import unittest
import numpy as np
import time
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.spacecraft.spacecraft import Spacecraft
import logging
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.kernel.config_loader import load_config

from alea.sim.utils.test_scenarios import create_aleasim_test_kernel

from alea.sim.kernel.generic.abstract_model import ModelNotFoundError
class KernelTest(unittest.TestCase):
    
    #TODO this test fails after spacecraft was updarted to pull in actuators
    # need to fix this test it currently fails
    def _test_kernel_model_creation(self):
        #create a kernel
        kernel = AleasimKernel(dt=0.1)
        
        #create an attitude dynamics model
        adyn = AttitudeDynamicsModel(kernel , init_state=np.array([1.0,0,0,0, 25, 1, -10]))
        kernel.add_model(adyn)
        
        odyn = OrbitDynamicsModel(kernel)
        kernel.add_model(odyn)

        #create the spacecraft model
        spacecraft: Spacecraft = Spacecraft(kernel)
        kernel.add_model(spacecraft)
        
        magm = EarthMagneticFieldModel(kernel)
        kernel.add_model(magm)
        
        #confirm that the connect method of the models has not been called yet
        #the spacecraft model will not have the dynamics models as attributes at the moment
        assert not hasattr(spacecraft, "_attitude_dynamics")
        assert not hasattr(spacecraft, "_orbit_dynamics")

        #advance the simulation scheudler by 1 timestep
        #what should happen now is that all the connect functions are called and the dynamics starts propagating
        kernel.step()
        
        #assert that the spacecraft model now has the desired attributes
        assert hasattr(spacecraft, "_attitude_dynamics")
        assert hasattr(spacecraft, "_orbit_dynamics")
        kernel.kill()
        
    def test_kernel_scheduler_1(self):
        kernel = AleasimKernel(dt='0.10001548391')
        # #create an attitude dynamics model
        adyn = AttitudeDynamicsModel(kernel , init_state=np.array([1.0,0,0,0, 25, 1, -10]))
        kernel.add_model(adyn)

        magm = EarthMagneticFieldModel(kernel)
        kernel.add_model(magm)
        
        odyn = OrbitDynamicsModel(kernel)
        kernel.add_model(odyn)

        start = time.time()
        kernel.advance(10)
        duration = time.time()-start
        
        print(kernel.time)

        print(f'simulated 10 seconds of sim time in {duration} s with simulated time step of {0.1}s')
        kernel.kill()

    def test_kernel_time_1(self):
        kernel = AleasimKernel(dt='0.1')

        adyn = AttitudeDynamicsModel(kernel)
        odyn = OrbitDynamicsModel(kernel)

        kernel.add_model(odyn)
        kernel.add_model(adyn)

        t0 = kernel.time
        t0_sf = kernel.skyfield_time
        date0 = kernel.date

        kernel.step()

        assert float(kernel.time - t0) == 0.1
        val = (kernel.date - date0).total_seconds()
        assert (kernel.date - date0).total_seconds() == 0.1
        assert kernel.skyfield_time > t0_sf
        print(kernel.date, date0)
        print(kernel.time, t0)
        print(kernel.skyfield_time, t0_sf)
    
        t0 = kernel.time
        t0_sf = kernel.skyfield_time
        date0 = kernel.date
        
        kernel.advance(10)

        kernel.kill()

    def test_kernel_model_tree(self):
        kernel = AleasimKernel(dt='0.1')

        adyn = AttitudeDynamicsModel(kernel)
        odyn = OrbitDynamicsModel(kernel)

        kernel.add_model(odyn)
        kernel.add_model(adyn)
        
        kernel.get_model(AttitudeDynamicsModel)
        
        from alea.sim.kernel.generic.abstract_model import ModelNotFoundError
        try:
            kernel.get_model('a')
        except Exception as err:
            assert isinstance(err, ModelNotFoundError)
        
        kernel.kill()
    
    def test_set_log_level_all(self):
        from alea.sim.aleasim import create_aleasim_control_demo
        
        kernel = create_aleasim_control_demo()
        
        kernel.set_log_level_all(logging.INFO)

        for model in kernel.root._children.values():
            assert model.logger.level == logging.INFO
        
        kernel.step()

        kernel.set_log_level_all(logging.DEBUG)
        
        for model in kernel.root._children.values():
            assert model.logger.level == logging.DEBUG
        
        kernel.step()
        
        kernel.kill()

    def test_config_load(self):
        cfg = load_config('template')
        assert type(cfg) is dict
        
        expected_cfg = {"config_param_1":"string value", "config_param_float":10.0, "config_param_float2":1e-4, "config_sub_dictionary": {"value1":10.0,}}
        
        #compare dicts by value
        assert cfg == expected_cfg

    def test_kernel_remove_mdl(self):        
        kernel = create_aleasim_test_kernel()
        kernel.step() #connect functions called

        sc: Spacecraft = kernel.get_model(Spacecraft)
        mag = sc._mag_sens

        assert mag is kernel.get_model(mag.name)
        
        kernel.remove_model(mag)
        
        try:
            kernel.get_model(mag.name)
        except ModelNotFoundError:
            pass
        
        kernel.kill()
                

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    unittest.main(verbosity=100)