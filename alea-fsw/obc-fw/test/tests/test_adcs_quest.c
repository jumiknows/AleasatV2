/**
 * @file test_sequence.c
 * @brief Unit tests for sequence.c module
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "unity.h"

#include "dsp_math.h"
//adcs quest functions
#include "quest.h"

//adcs quaternion math
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
TEST_FILE("arm_math.h")
TEST_FILE("quest.c")

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define TOLERANCE_QUAT_ELEMENT 1E-5

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

//test input structure
typedef struct
{
    adcs_ad_triax_vectors_t ad_vecs;
    float32_t true_q[4];
} quest_test_input_t;

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static quest_test_input_t test_input1 = {
    .true_q = {0.8736198818292642, -0.4675471718472249, -0.06363602741409172, 0.11890500487884104},

    .ad_vecs = {
        .mag_obs = {11080.38102839469,32810.40881952397,-32549.68097631773},
        .sun_obs = {0.39163203563598614,0.13463786712340367,0.910218102105245},
        .mag_ref = {13051.412336335856,-5598.546872719363,-45355.22474026194},
        .sun_ref = {0.15501654647973498,0.9064331102385519,0.39287260909964367}
    }
};

// make sure the answer is accurate up to 4th digit
float32_t delta = 0.0001;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void setUp(void) {
}

void tearDown(void) {
}

void test_quest_1(void) {
    float32_t est_q[4] = {0.0f};
    float32_t weights[2] = {0.5f, 0.5f};

    quest_estimate(&(test_input1.ad_vecs), (float32_t *)weights, est_q);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_QUAT_ELEMENT, 0.0f, quaternion_abs_angle_diff(test_input1.true_q, est_q));
}
