# ==============================================================================
# Import Root Model
# ==============================================================================

from alea.sim.kernel.root_models import AleasimRootModel
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.spacecraft.sensors.simple_mag_sensor import SimpleMagSensor
from alea.sim.epa.earth_magnetic_field import EarthMagneticFieldModel
from alea.sim.kernel.frames import *
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.spacecraft.spacecraft import Spacecraft
from alea.sim.spacecraft.aocs import AOCSModel
from alea.sim.spacecraft.actuators.reaction_wheel import ReactionWheelModel
from alea.sim.spacecraft.eps.solar_panel import SolarPanelModel
from alea.sim.spacecraft.eps.power_system import PowerSystemModel
from alea.sim.spacecraft.eps.eps import EPSModel
from alea.sim.spacecraft.spacecraft import Spacecraft

import matplotlib.pyplot as plt
import numpy as np

import json
import logging

def run():
    logging.disable()

    # ==============================================================================
    # Configure Sim and Grab models from the kernel
    # ==============================================================================

    kernel = AleasimKernel(dt=1e-1, date=2024.2)
    root_model = AleasimRootModel(kernel)
    kernel.add_model(root_model)

    adyn: AttitudeDynamicsModel = kernel.get_model(AttitudeDynamicsModel)
    odyn: OrbitDynamicsModel = kernel.get_model(OrbitDynamicsModel)
    sc: Spacecraft = kernel.get_model(Spacecraft)

    rwx, rwy, rwz = sc.aocs._rws
    mtqx, mtqy, mtqz =  sc.aocs._mtqs

    mag_sens = sc.aocs._mag_sens
    sun_sens = sc.aocs._sun_sens
    gyro = sc.aocs._gyro_sens

    power_sys: PowerSystemModel = kernel.get_model(PowerSystemModel)

    solar_panels: list[SolarPanelModel] = power_sys.solar_panels
    eps = power_sys.eps

    # ==============================================================================
    # Configure
    # ==============================================================================

    eps._force_state_of_charge(50)

    adyn.set_state(np.array([0.0, 0.0, 0.0, 1.0,0,0.5,1.0]))

    kernel.advance_n(2)
    sc.aocs.set_mode(sc.aocs.AOCSMode.POINTING_SUN_A, 0)
    sc.aocs.set_mode(sc.aocs.AOCSMode.POINTING_CAMERA_NADIR,6000)

    # ==============================================================================
    # Run the control demo for 10 seconds
    # ==============================================================================

    kernel.advance(1000)

    kernel.kill()

import cProfile
import pstats
from pstats import SortKey
from pstats import Stats
if __name__ == '__main__':
    logging.disable()
    pr = cProfile.Profile()
    pr.enable()
    run()
    pr.disable()

    stats = Stats(pr)
    stats.sort_stats('tottime').print_stats(50)
    filename = 'profile.prof'
    pr.dump_stats(filename)