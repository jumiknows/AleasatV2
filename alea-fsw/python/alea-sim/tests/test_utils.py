from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.kernel.frame_manager import FrameManager
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel

from alea.sim.kernel.kernel import AleasimKernel

from alea.sim.spacecraft.actuators.simple_actuators import SimpleActuator, SimpleMagnetorquer
from alea.sim.spacecraft.sensors.simple_sensors import SimpleGyroSensor, SimpleMagSensor, SimpleSunSensor
from alea.sim.spacecraft.spacecraft import Spacecraft

from alea.sim.epa.disturbance_model import DisturbanceModel

def create_aleasim_test_kernel() -> AleasimKernel:
    """Setup kernel scenario for test cases"""
    dt = 0.01
    sample_rate = int(1/dt)
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
    
    mag_sens = SimpleMagSensor('mag_sens', kernel, sample_rate=sample_rate)
    sun_sens = SimpleSunSensor('sun_sens', kernel, sample_rate=sample_rate)
    gyro = SimpleGyroSensor('gyro', kernel, sample_rate=sample_rate)


    kernel.add_model(mag_sens)
    kernel.add_model(sun_sens)
    kernel.add_model(gyro)


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

    adyn._integrator_type = "rk45"

    return kernel