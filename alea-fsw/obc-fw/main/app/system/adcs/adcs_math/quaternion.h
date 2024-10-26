/**
 * @file quaternion.h
 * @brief Functions for manipulating quaternions (float32_t[4])
 * https://en.wikipedia.org/wiki/Quaternion
 */

#ifndef QUATERNION_H_
#define QUATERNION_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

//DSPLIB TYPES
#include "type_defs.h"

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/**
  @brief         Floating-point quaternion Norm.
  @param         quat        points to the input quaternion
  @return        float norm
 */
float32_t quaternion_norm(const float32_t *quat);

/**
  @brief         Norm squared of the quaternion.
  @param         quat        points to the input quaternion
  @return        float norm squared
 */
float32_t quaternion_norm2(const float32_t *quat);

/**
  @brief         Normalize the floating-point quaternion.
  @param         quat        points to the input quaternion
  @return        None
 */
void quaternion_normalize(float32_t *quat);

/**
  @brief         Conjugate of the floating-point quaternion
  @param         quat        points to the input quaternion
  @param         q_dest      points to output quaternion to store conjugate.

  @note The conjugate of a unit quaternion is equal to the inverse.
 */
void quaternion_conjugate(const float32_t *quat, float32_t *q_dest);

/**
  @brief         Product of 2 quaternions
  @param         q1          points to input quaternion 1
  @param         q1          points to input quaternion 2
  @param         q_dest      points to output quaternion
  @return        None
 */
void quaternion_product(const float32_t *q1, const float32_t *q2, float32_t *q_dest);

/**
  @param         q1         points to input quaternion 1
  @param         q1         points to input quaternion 2
  @return        Absolute angle difference between q1 and q2 in radians.
 */
float32_t quaternion_abs_angle_diff(const float32_t *q1, const float32_t *q2);

/**
  @brief         transforms/rotates a vector in accordance with a given quaternion
  @param         q         a unit quaternion that is used to transform vectors
  @param         v         vector to transform
  @param         v_dest    points to output vector
  @return        None
 */
void quaternion_transform(const float32_t *q, const float32_t *v, float32_t *v_dest);

#endif //QUATERNION_H
