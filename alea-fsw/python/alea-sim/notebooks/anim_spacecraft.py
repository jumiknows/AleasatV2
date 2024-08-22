import unittest
from parameterized import parameterized

from datetime import datetime

import numpy as np
import time

from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.kernel.kernel import AleasimKernel
# from aleasim.kernel.graphics.anim_attitude import AttitudeAnimation

#graphics/game engine
from ursina import *


kernel = AleasimKernel(dt=1e-4,time_precision=5) #init kernal at the day this test was created
adyn_model = AttitudeDynamicsModel(kernel, np.array([1,0,0,0,10,100.0,3]))
print(kernel.time)
print(kernel.timestep) 

# create a window
app = Ursina()

cubesat = Entity(model='cube', texture='white_cube', color=color.hsv(0, 0, random.uniform(.9, 1.0)), highlight_color=color.lime, position=(0,0,0))
# arrow = Entity(model='arrow', position=(-0.5,0,0))
rot_info = Text('rotation: ', origin=(1.5,3))

def update():
    q1 = adyn_model.quaternion
    kernel.step()
    q2 = adyn_model.quaternion
    diff = q2 * q1.inverse
    angles = q2.to_euler()
    att_quat = q2.to_array()
    cubesat.quaternion_setter(Quat(att_quat[0], att_quat[1], att_quat[2], att_quat[3]))
    rot_info.text = f'time: {kernel.time}\n' + f'rotation: {adyn_model.quaternion}\n' + \
                    f'angular vel: {adyn_model.angular_velocity} rad/s\n' + \
                    f'angular accel: {adyn_model.angular_acceleration} rad/s2\n' + \
                    f'kinetic energy: {adyn_model.calculate_rotational_kinetic_energy()} J'

def input(key):
    pass

camera.orthographic = True
EditorCamera()

# start running the game
app.run(info=True)
