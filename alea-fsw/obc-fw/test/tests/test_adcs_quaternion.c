/**
 * @file test_adcs_quaternion.c
 * @brief Unit tests for quaternion.c module
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "unity.h"

#include "dsp_math.h"

//adcs quaternion functions
#include "quaternion.h"

TEST_FILE("arm_mat_mult_f32.c")
TEST_FILE("arm_mat_scale_f32.c")
TEST_FILE("arm_mat_add_f32.c")
TEST_FILE("arm_mat_trans_f32.c")
TEST_FILE("arm_mat_add_f32.c")
TEST_FILE("arm_mat_inverse_f32.c")
TEST_FILE("arm_atan2_f32.c")
TEST_FILE("arm_abs_f32.c")
TEST_FILE("quaternion.c")

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// tests pass with a maximum of 5 digit precision
#define DELTA 0.000001

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static inline void testVector(float32_t v1[3], float32_t v2[3]);

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void test_quaternion_transform_simple(void) {
    // simple quaternion test case (8, 12, 16) angle pi/3
    float32_t q[4] = {0.866025403784439, 0.185695338177052, 0.278543007265578, 0.371390676354104};
    float32_t input[3] = {3, 9, 7};
    float32_t dest[3] = {0, 0, 0};
    float32_t ans[3] = {6.015701479247106, 7.976806174244443, 6.259544629693115};

    quaternion_transform(q, input, dest);
    testVector(ans, dest);
}

void test_quaternion_transform_complex(void) {
    // complex quaternion test case with (7.12 1.23 0.48) and angle of 3*pi/15
    float32_t q[4] = {0.951056516295154, 0.303836916823623, 0.052488680855766, 0.020483387651031};
    float32_t input[3] = {6.123432, 0.18263, 6.172532};
    float32_t dest[3] = {0, 0, 0};
    float32_t ans[3] = {5.558062413446144, 3.686068120433042, 5.581287350272559};

    quaternion_transform(q, input, dest);
    testVector(ans, dest);
}

void test_quaternion_transform_null_angle(void) {
    // test with an angle of 0
    float32_t q[4] = {1, 0, 0, 0};
    float32_t input[3] = {7, 18, 32};
    float32_t dest[3] = {0, 0, 0};
    float32_t ans[3] = {7, 18, 32};

    quaternion_transform(q, input, dest);
    testVector(ans, dest);
}

void test_quaternion_transform_linearly_dependent(void) {
    // test with a linearly dependent quaternion (1,2,3) angle pi/2
    float32_t q[4] = {0.707106781186548, 0.188982236504614, 0.377964473009227, 0.566946709513841};
    float32_t input[3] = {2, 4, 6};
    float32_t dest[3] = {0, 0, 0};
    float32_t ans[3] = {2, 4, 6};

    quaternion_transform(q, input, dest);
    testVector(ans, dest);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

static inline void testVector(float32_t v1[3], float32_t v2[3]) {
    TEST_ASSERT_FLOAT_WITHIN(DELTA, v1[0], v2[0]);
    TEST_ASSERT_FLOAT_WITHIN(DELTA, v1[1], v2[1]);
    TEST_ASSERT_FLOAT_WITHIN(DELTA, v1[2], v2[2]);
}