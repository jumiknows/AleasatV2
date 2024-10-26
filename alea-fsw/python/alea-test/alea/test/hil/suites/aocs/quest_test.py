import numpy as np
import ahrs

from alea.test import timeout
from alea.test.hil import hil_test
from alea.test.stk.stk_data_reader import STKDataReader

class QuestTest(hil_test.HILTest):
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
        with STKDataReader.data_file_path("ad_stk_sim_data.csv") as data_file_path:
            with STKDataReader.data_file_path("ad_stk_sim_data_initial_conditions.json") as ic_file_path:
                stk_data = STKDataReader(data_file_path, ic_file_path)

        #sample the data every 50 time steps, we don't need to test all values
        #this dataset has 1001 points
        for i in np.arange(0,stk_data.size,25):
            t = round(i*stk_data.time_step,3)
            resp = self.ttc.send_obc_cmd("TEST_QUEST", stk_data.get_sun_vector_ref(t).tolist(), 
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
    hil_test.main(QuestTest)
