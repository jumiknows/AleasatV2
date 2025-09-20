/**
 * @file test_adcs_frames.c
 * @brief Unit tests for NEDtoECI.c module
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "unity.h"

#include "dsp_math.h"

#include "NEDtoECI.h"

TEST_FILE("arm_mat_mult_f32.c")
TEST_FILE("arm_mat_scale_f32.c")
TEST_FILE("arm_mat_add_f32.c")
TEST_FILE("arm_mat_trans_f32.c")
TEST_FILE("arm_mat_add_f32.c")
TEST_FILE("arm_mat_inverse_f32.c")
TEST_FILE("arm_atan2_f32.c")
TEST_FILE("arm_abs_f32.c")

TEST_FILE("arm_sin_f32.c")
TEST_FILE("arm_cos_f32.c")
TEST_FILE("arm_math.h")
TEST_FILE("NEDtoECI.c")

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/
// accurate to 10 units
#define DELTA 10

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static inline void testVector(float32_t v1[3], float32_t v2[3]);

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct {
    char* name;
    float32_t input[3];
    float32_t lat;
    float32_t lon;
    float32_t altitude;
    float32_t time;
    float32_t position[3];
    float32_t answer[3];
} adcs_frame_testcase_t;

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

const static adcs_frame_testcase_t test_cases[] = {
    {
        "Basic no ECI",
        {1.0, 2.0, 3.0}, // input vector
        50.0, 50.0, 450.0, // lat, lon, altitude
        0.0, // seconds
        {2826.41330012892, 3368.38820458469, 5207.50903710997}, // position
        {2823.14927963267, 3367.60974408571, 5205.8536913903} // result
    },
    {
        "Basic with ECI",
        {12, 4.21, 3.912},
        49.2827, 123.1207, 519.2167,
        11631.612,
        {-2462.83967176646, 3775.00650917274, 5204.66022766836},
        {1195.63257077275, 4333.80592660178, 5209.52310397701}
    },
    {
        "Negative lat/lon",
        {41.918, 13.21, 5.96},
        -84.3234, -12.7831, 519.2167,
        11631.612,
        {667.380218990445, -151.418067696394, -6842.04106658337},
        {359.083065148212, -630.502180584499, -6831.96404416301}
    },
    {
        "Position above equator",
        {41.918, 13.21, 5.96},
        0, 101.795, 302.863,
        23621,
        {-1365.66741080478, 6539.93222618291, 0.0},
        {6664.5271273266, 374.715458485255, 41.918}
    },
    {
        "Position at lat/lon = 0",
        {41.918, 13.21, 5.96},
        0, 0, 302.863,
        23621,
        {6681, 0.0, 0.0},
        {-995.49675252473, -6600.40299689441, 41.918}
    },
    {
        "Time = 1 day",
        {12, 4.21, 3.912},
        49.2827, 123.1207, 519.2167,
        86400,
        {-2462.83967176646, 3775.00650917274, 5204.66022766836},
        {-2394.90736250482, 3804.71152372444, 5209.52310397701}
    }
};
const static len = 6;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void test_NEDtoECI(void) {
    for (int i = 0; i < len; i++) {
        adcs_frame_testcase_t t = test_cases[i];
        float32_t pos[3];
        printf("running %s...\n", t.name);
        NEDtoECI(t.input, t.position, t.lat, t.lon, t.time, &pos);
        testVector(t.answer, pos);
        printf("passed\n");
    }
}

void test_get_ECEF(void) {
    for (int i = 0; i < len; i++) {
        adcs_frame_testcase_t t = test_cases[i];
        float32_t pos[3];
        printf("running %s...\n", t.name);
        get_ECEF(t.lat, t.lon, t.altitude, &pos);
        testVector(t.position, pos);
        printf("passed\n");
    }
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

static inline void testVector(float32_t v1[3], float32_t v2[3]) {
    TEST_ASSERT_FLOAT_WITHIN(DELTA, v1[0], v2[0]);
    TEST_ASSERT_FLOAT_WITHIN(DELTA, v1[1], v2[1]);
    TEST_ASSERT_FLOAT_WITHIN(DELTA, v1[2], v2[2]);
}