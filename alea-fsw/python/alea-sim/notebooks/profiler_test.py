from alea.sim.epa.disturbance_model import DisturbanceModel
from alea.sim.math_lib.math_ahrs import AHRSQuaternion

import logging

import alea.sim
import numpy as np
import scipy as sp
import skyfield

from datetime import datetime
import time
from alea.sim.epa.magnetic_field_model import EarthMagneticFieldModel
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.spacecraft.spacecraft import Spacecraft
import matplotlib.pyplot as plt
import logging
import cProfile
import pstats
from pstats import SortKey
from pstats import Stats
from matplotlib import pyplot as plt
import skyfield.sgp4lib

from typing import Dict

import numpy as np


from alea.sim.kernel.frames import ReferenceFrame, FrameTransformation
from alea.sim.spacecraft.actuators.simple_actuators import SimpleActuator, SimpleMagnetorquer
from alea.sim.spacecraft.sensors.simple_sensors import SimpleMagSensor, SimpleSunSensor, SimpleGyroSensor


import logging
import cProfile
import pstats
from pstats import SortKey

def create_scenario(sim_dt=1e-3, control_dt=0.1, st=0.1e-3, sm=0) -> AleasimKernel:
    control_sample_rate = int(1/control_dt)
    kernel = AleasimKernel(dt=sim_dt, date=2024.0)

    adyn = AttitudeDynamicsModel(kernel)
    odyn = OrbitDynamicsModel(kernel)

    magm = EarthMagneticFieldModel(kernel)

    rwx = SimpleActuator('rw_x', kernel, st)
    rwy = SimpleActuator('rw_y', kernel, st)
    rwz = SimpleActuator('rw_z', kernel, st)

    mtqx = SimpleMagnetorquer('mtq_x',kernel, sm)
    mtqy = SimpleMagnetorquer('mtq_y',kernel, sm)
    mtqz = SimpleMagnetorquer('mtq_z',kernel, sm)

    sc = Spacecraft(kernel, ctrl_sample_period=control_dt)

    DModel = DisturbanceModel(kernel)

    mag_sens = SimpleMagSensor('mag_sens', kernel, sample_rate=control_sample_rate)
    sun_sens = SimpleSunSensor('sun_sens', kernel, sample_rate=control_sample_rate)
    gyro = SimpleGyroSensor('gyro', kernel, sample_rate=control_sample_rate)
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
    kernel.add_model(mag_sens)
    kernel.add_model(sun_sens)
    kernel.add_model(gyro)
    adyn._integrator_type = "rk45"
    adyn.set_state(np.array([0.1,0.756,0.1,0,0,0,0.0]))
    kernel.set_log_level_all(logging.INFO)
    kernel.step()
    sc._use_quest = True
    sc._use_lpf = False
    
    return kernel

if __name__ == '__main__':
    logging.disable()
    kernel = create_scenario(sim_dt=1e-2, st=1e-3)
    sim_seconds = 1.0

    start = time.monotonic()
    
    pr = cProfile.Profile()
    pr.enable()

    start = time.monotonic()
    kernel.advance(sim_seconds)
    duration = time.monotonic() - start
    pr.disable()
    kernel.kill()

    print(f'executed {sim_seconds} simulation second in {duration} realtime seconds\nsimulation speed factor: {sim_seconds/duration}\n')
    if kernel.scheduler._debug_realtime:
        print(f'mean loop duration for scheduler: {np.mean(kernel.scheduler._debug_time_arr)}\n')

        for func_name in kernel.scheduler._debug_time_map.keys():
            print(f'{func_name} :\t\t per call = {np.mean(kernel.scheduler._debug_time_map[func_name]):.10f} s , total = {np.sum(kernel.scheduler._debug_time_map[func_name]):.10f} s')
    stats = Stats(pr)
    stats.sort_stats('tottime').print_stats(25)
    
    # kernel = create_scenario(0.001)
    # adyn = kernel.get_model(AttitudeDynamicsModel)
    # sc = kernel.get_model(Spacecraft)
    # kernel.step()

    # pr = cProfile.Profile()
    # pr.enable()

    # # adyn.update()
    # kernel.frame_manager.update_frames()

    # pr.disable()
    # stats = Stats(pr)
    # stats.sort_stats('tottime').print_stats(50)
    
    