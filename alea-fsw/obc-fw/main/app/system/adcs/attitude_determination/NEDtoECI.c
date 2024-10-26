/**
 * @file NEDtoECI.c
 * @brief source code
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// DSPLIB types
#include "type_defs.h"
#include "dsp_math.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define ROTATION 0.000072921159f // (rad/s)
#define E2 0.00669437985f
#define A_CONST 6378.137f
#define RADIANS_TO_DEGREES 0.017453292f
#define PI_CONST 3.14159265359f
#define DEGREES_TO_RADIANS (PI_CONST / 180.0f)


/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

// Typedefs that are only used in this file

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

// Prototypes for functions only used in this file (always declare static)

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

// Global variables that are only used in this file (always declare static)

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

// Global variable definitions matching the extern definitions in the header

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

// we do not have to use lat/lon if given a position ecef vector
// however, I do not see a use case where we have a position vector but not lat/lon,
// and the equations to convert from position vector to lat/lon are large
// so why not save on processing
void NEDtoECI(const float32_t *input, const float32_t *position, float32_t lat, float32_t lon, float32_t time, float32_t *dest) {
    // precompute
    float32_t latr = lat * DEGREES_TO_RADIANS;
    float32_t lonr = lon * DEGREES_TO_RADIANS;

    float32_t sinLat = arm_sin_f32(latr);
    float32_t sinLon = arm_sin_f32(lonr);
    float32_t cosLat = arm_cos_f32(latr);
    float32_t cosLon = arm_cos_f32(lonr);

    float32_t sinV = arm_sin_f32(ROTATION * time);
    float32_t cosV = arm_cos_f32(ROTATION * time);

    // NED to ECEF
    float32_t ecef[3];
    ecef[0] = (-1 * sinLat * cosLon * input[0] - sinLon * input[1] - cosLat * sinLon * input[2]);
    ecef[1] = (-1 * sinLat * sinLon * input[0] + cosLon * input[1] - cosLat * sinLon * input[2]);
    ecef[2] = (cosLat * input[0] - sinLat * input[2]);

    // add
    ecef[0] = ecef[0] + position[0];
    ecef[1] = ecef[1] + position[1];
    ecef[2] = ecef[2] + position[2];

    // ECEF to ECI
    dest[0] = cosV * ecef[0] + sinV * ecef[1];
    dest[1] = -1 * sinV * ecef[0] + cosV * ecef[1];
    dest[2] = ecef[2];

    return;
}


void get_ECEF(float32_t lat, float32_t lon, float32_t altitude, float32_t *dest) {
    // precompute
    float32_t latr = lat * DEGREES_TO_RADIANS;
    float32_t lonr = lon * DEGREES_TO_RADIANS;

    float32_t sinLat = arm_sin_f32(latr);
    float32_t sinLon = arm_sin_f32(lonr);
    float32_t cosLat = arm_cos_f32(latr);
    float32_t cosLon = arm_cos_f32(lonr);

    float32_t RN;
    arm_sqrt_f32(A_CONST * A_CONST / (1 - (E2 * sinLat * sinLat)), &RN);

    // compute
    dest[0] = (RN + altitude) * cosLat * cosLon;
    dest[1] = (RN + altitude) * cosLat * sinLon;
    dest[2] = (RN * (1 - E2) + altitude) * sinLat;

    return;
}


/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

// Implementations for functions only used in this file

