from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.kernel.frame_manager import FrameManager
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel

from alea.sim.kernel.kernel import AleasimKernel

from alea.sim.spacecraft.actuators.simple_actuators import SimpleActuator, SimpleMagnetorquer
from alea.sim.spacecraft.sensors import SimpleGyroSensor, SimpleMagSensor, SimpleSunSensor
from alea.sim.spacecraft.spacecraft import Spacecraft

from alea.sim.epa.disturbance_model import DisturbanceModel

def create_aleasim_test_kernel() -> AleasimKernel:
    """Setup kernel scenario for test cases"""
    dt = 0.01
    kernel = AleasimKernel(dt=dt, date=2024.2)
    adyn = AttitudeDynamicsModel(kernel)
    odyn = OrbitDynamicsModel(kernel)
    magm = EarthMagneticFieldModel(kernel)
    sc = Spacecraft(kernel, ctrl_sample_period=dt)
    DModel = DisturbanceModel(kernel)
    kernel.add_model(magm)
    kernel.add_model(odyn)
    kernel.add_model(adyn)
    kernel.add_model(DModel)
    kernel.add_model(sc)

    return kernel