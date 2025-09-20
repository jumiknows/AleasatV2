from alea.sim.kernel.root_models import AleasimRootModel
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.epa.attitude_dynamics import AttitudeDynamicsModel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel
from alea.sim.spacecraft.spacecraft import Spacecraft
from alea.sim.math_lib import Quaternion

import os
import numpy as np
import pandas as pd
import csv

def get_ECEF(start_data, sim_run_time):
    # Clear the CSV file before running the script
    csv_file = os.path.join(os.path.dirname(__file__), '../data/ecef_time_orbit_data.csv')
    csv_file = os.path.abspath(csv_file)  
    with open(csv_file, 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['time', 'ecef_x', 'ecef_y', 'ecef_z']) # add col names

    # Set the simulation params
    kernel = AleasimKernel(dt=0.1, date=start_data)
    root_model = AleasimRootModel(kernel)
    kernel.add_model(root_model)

    adyn: AttitudeDynamicsModel = kernel.get_model(AttitudeDynamicsModel)
    odyn: OrbitDynamicsModel = kernel.get_model(OrbitDynamicsModel)
    sc: Spacecraft = kernel.get_model(Spacecraft)

    adyn.set_state(np.array([0.0, 0.0, 0.0, 1.0,0,0.5,1.0]))

    kernel.advance_n(2)
    sc.aocs.set_mode(sc.aocs.AOCSMode.POINTING_CAMERA_NADIR)
    sc.aocs.set_mode(sc.aocs.AOCSMode.POINTING_MIN_POWER_GEN,500)
    sc.aocs.set_mode(sc.aocs.AOCSMode.MOMENTUM_DUMP, 1000)
    sc.aocs.set_mode(sc.aocs.AOCSMode.POINTING_SUN_A, 2000)
    sc.aocs.set_mode(sc.aocs.AOCSMode.POINTING_QUAT, t_delay=3000, q=Quaternion.random())

    # Run the simulation
    kernel.advance(sim_run_time)

    # Save the data
    size = odyn.state_array.shape[0]
    ecef_array = np.empty((size, 3))

    for i in range(size):
        ecef_array[i] = kernel.body_frame.transform_vector_from_frame(
            odyn.state_array[:, 15:18][i], 
            kernel.ecef_frame
        )

    df = pd.DataFrame({
        'time': odyn.time_array,
        'ecef_x': ecef_array[:, 0],
        'ecef_y': ecef_array[:, 1],
        'ecef_z': ecef_array[:, 2]
    })

    path = os.path.join(os.getcwd(), 'data', 'ecef_time_orbit_data.csv')
    df.to_csv(path, index=False)

if __name__ == "__main__":
    get_ECEF(2024.2, 10)