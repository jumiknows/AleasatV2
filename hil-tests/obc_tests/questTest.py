from obc_test_internals import obc_test, timeout
from parameterized import parameterized
from datetime import date
from obcpy.cmd_sys.resp import *
import numpy as np
from adcs_test_internals.stk_data_reader import STKDataReader
import ahrs
import os

class QuestTest(obc_test.OBCTest):
    ANGLE_ERROR_TOLERANCE = 1e-3 #degree
    
    #absolute angle error between 2 quaternions in degrees
    @classmethod
    def abs_ang_diff(self,q1:np.ndarray,q2:np.ndarray):
        quat1 = ahrs.Quaternion(q1)
        quat2 = ahrs.Quaternion(q2)
        q_diff = ahrs.Quaternion(ahrs.Quaternion(quat1.conjugate).product(quat2))
        # print(q_diff)
        _,angle = q_diff.to_axang()
        angle = abs(angle) * 180 / np.pi
        if angle > 180:
            angle -= 360
            angle = abs(angle)
        return angle

    #this test loops through many datapoints
    @timeout.timeout(30)
    def test_quest_no_noise(self):
        obc_tests_path = os.path.dirname(os.path.realpath(__file__))
        stk_data = STKDataReader(os.path.join(obc_tests_path,'adcs_test_internals/ad_stk_sim_data.csv'), 
                                  os.path.join(obc_tests_path,'adcs_test_internals/ad_stk_sim_data_initial_conditions.json'))

        #sample the data every 50 time steps, we don't need to test all values
        #this dataset has 1001 points
        for i in np.arange(0,stk_data.size,25):
            t = round(i*stk_data.time_step,3)
            resp = self.obc.send_cmd("TEST_QUEST", stk_data.get_sun_vector_ref(t).tolist(), 
                                     stk_data.get_sun_vector_body(t).tolist(), 
                                     stk_data.get_mag_vector_ref(t).tolist(),
                                     stk_data.get_mag_vector_body(t).tolist(),
                                     [0.5,0.5])
            self.assertTrue(resp.data["adcs_err"] == 0)

            q_true = stk_data.get_attitude_quaternion(t)

            #stk gives us intertial->body quaternion but quest gives us the inverse(conjugate) of this
            #so just inverse the true quaternion
            q_true = np.array([q_true[0], -q_true[1], -q_true[2], -q_true[3]])

            angle_err = self.abs_ang_diff(q_true, np.array(resp.data["quat"]))
            
            print(f'True Q: {q_true} , Estimated Q: {np.array(resp.data["quat"])}, Angle Error: {angle_err}')
            print(f'Average execution time: {resp.data["avg_duration"]} us')

            self.assertTrue(angle_err <= self.ANGLE_ERROR_TOLERANCE)

if __name__ == '__main__':
    obc_test.main(QuestTest)
