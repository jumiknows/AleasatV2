from aleasim.epa.attitude_dynamics import AttitudeDynamicsModel
from aleasim.epa.orbit_dynamics import OrbitDynamicsModel
from aleasim.kernel.frame_manager import FrameManager
from aleasim.epa.magnetic_field_model import EarthMagneticFieldModel
from aleasim.kernel.kernel import AleasimKernel
from aleasim.spacecraft.actuators.simple_actuators import SimpleActuator, SimpleMagnetorquer
from aleasim.spacecraft.sensors.simple_sensors import SimpleGyroSensor, SimpleMagSensor, SimpleSunSensor
from aleasim.spacecraft.spacecraft import Spacecraft
from aleasim.epa.disturbance_model import DisturbanceModel

def create_aleasim_control_demo() -> AleasimKernel:
    dt = 0.01
    kernel = AleasimKernel(dt=str(dt))

    adyn = AttitudeDynamicsModel(kernel)
    odyn = OrbitDynamicsModel(kernel)

    magm = EarthMagneticFieldModel(kernel)

    st = 1e-3
    sm = 0.1
    rwx = SimpleActuator('rw_x', kernel, st)
    rwy = SimpleActuator('rw_y', kernel, st)
    rwz = SimpleActuator('rw_z', kernel, st)

    mtqx = SimpleMagnetorquer('mtq_x',kernel, sm)
    mtqy = SimpleMagnetorquer('mtq_y',kernel, sm)
    mtqz = SimpleMagnetorquer('mtq_z',kernel, sm)

    sc = Spacecraft(kernel, ctrl_sample_period=dt)

    DModel = DisturbanceModel(kernel)


    kernel.add_model(magm)
    kernel.add_model(odyn, True)
    kernel.add_model(adyn, True)

    kernel.add_model(DModel)

    kernel.add_model(rwx)
    kernel.add_model(rwy)
    kernel.add_model(rwz)
    kernel.add_model(mtqx)
    kernel.add_model(mtqy)
    kernel.add_model(mtqz)
    kernel.add_model(sc, True)
    
    

    return kernel