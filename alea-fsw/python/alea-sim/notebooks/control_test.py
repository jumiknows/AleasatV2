from alea.sim.epa.disturbance_model import DisturbanceModel
from alea.sim.math_lib.math_ahrs import AHRSQuaternion

import logging

import alea.sim
import numpy as np
import scipy as sp
import skyfield

from datetime import datetime
import time
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.spacecraft.spacecraft import Spacecraft
import matplotlib.pyplot as plt
import logging
import cProfile
import pstats
from pstats import SortKey
from matplotlib import pyplot as plt
import skyfield.sgp4lib

from typing import Dict

import numpy as np


from alea.sim.kernel.frames import ReferenceFrame, FrameTransformation
from alea.sim.spacecraft.actuators.simple_actuators import SimpleActuator, SimpleMagnetorquer
from alea.sim.spacecraft.sensors.simple_sensors import SimpleMagSensor, SimpleSunSensor, SimpleGyroSensor

logging.basicConfig(level=logging.INFO)
kernel = AleasimKernel(dt='0.01')

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

sc = Spacecraft(kernel, ctrl_sample_period=0.01)

DModel = DisturbanceModel(kernel)

kernel.add_model(rwx)
kernel.add_model(rwy)
kernel.add_model(rwz)
kernel.add_model(mtqx)
kernel.add_model(mtqy)
kernel.add_model(mtqz)
kernel.add_model(sc, True)
kernel.add_model(magm)
kernel.add_model(odyn, True)
kernel.add_model(adyn, True)
kernel.add_model(DModel)

kernel.step()

adyn._integrator_type = "quad"

adyn.set_state(np.array([0.1,0.756,0.1,0,0,0,0.0]))

kernel.advance(10)

kernel.kill()