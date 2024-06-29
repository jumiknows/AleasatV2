/**
 * @file quaternion.h
 * @brief Functions for manipulating quaternions (float32_t[4])
 * https://en.wikipedia.org/wiki/Quaternion
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "quaternion.h"

//DSPLIB
#include "dsp_math.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

float32_t quaternion_norm(const float32_t *quat) {
    float32_t norm = -1.0f;
    arm_sqrt_f32(quaternion_norm2(quat), &norm);
    return norm;
}

float32_t quaternion_norm2(const float32_t *quat) {
    return ((quat[0] * quat[0]) + (quat[1] * quat[1]) + (quat[2] * quat[2]) + (quat[3] * quat[3]));
}

void quaternion_normalize(float32_t *quat) {
    float32_t norm = quaternion_norm(quat);
    quat[0] /= norm;
    quat[1] /= norm;
    quat[2] /= norm;
    quat[3] /= norm;
}

void quaternion_conjugate(const float32_t *quat, float32_t *q_dest) {
    q_dest[0] = quat[0];
    q_dest[1] = -1.0f * quat[1];
    q_dest[2] = -1.0f * quat[2];
    q_dest[3] = -1.0f * quat[3];
}

void quaternion_product(const float32_t *q1, const float32_t *q2, float32_t *q_dest) {
    q_dest[0] = ((q1[0] * q2[0]) - (q1[1] * q2[1]) - (q1[2] * q2[2]) - (q1[3] * q2[3]));
    q_dest[1] = ((q1[1] * q2[0]) + (q1[0] * q2[1]) - (q1[3] * q2[2]) + (q1[2] * q2[3]));
    q_dest[2] = ((q1[2] * q2[0]) + (q1[3] * q2[1]) + (q1[0] * q2[2]) - (q1[1] * q2[3]));
    q_dest[3] = ((q1[3] * q2[0]) - (q1[2] * q2[1]) + (q1[1] * q2[2]) + (q1[0] * q2[3]));
}

float32_t quaternion_abs_angle_diff(const float32_t *q1, const float32_t *q2) {
    float32_t temp[4];
    float32_t q_diff[4];
    quaternion_conjugate(q1, temp);
    quaternion_product(temp, q2, q_diff);
    arm_sqrt_f32(((q_diff[1]*q_diff[1]) + (q_diff[2]*q_diff[2]) + (q_diff[3]*q_diff[3])), &(temp[0]));
    arm_atan2_f32(temp[0], q_diff[0], &(temp[1]));
    temp[1] *= 2.0f;
    arm_abs_f32(&(temp[1]), &(temp[0]), 1u);
    return temp[0];
}

void quaternion_transform(const float32_t *q, const float32_t *v, float32_t *v_dest) {
    // first multiplication
    float32_t t[4];
    t[0] = (q[1] * v[0]) + (q[2] * v[1]) + (q[3] * v[2]);
    t[1] = (q[0] * v[0]) - (q[2] * v[2]) + (q[3] * v[1]);
    t[2] = (q[0] * v[1]) + (q[1] * v[2]) - (q[3] * v[0]);
    t[3] = (q[0] * v[2]) - (q[1] * v[1]) + (q[2] * v[0]);
    
    // second multiplication
    v_dest[0] = (t[0] * q[1]) + (t[1] * q[0]) + (t[2] * q[3]) - (t[3] * q[2]);
    v_dest[1] = (t[0] * q[2]) - (t[1] * q[3]) + (t[2] * q[0]) + (t[3] * q[1]);
    v_dest[2] = (t[0] * q[3]) + (t[1] * q[2]) - (t[2] * q[1]) + (t[3] * q[0]);
}
