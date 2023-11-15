/**
 * @file quest.h
 * @brief Quest attitude determination algorithm
 */

#ifndef QUEST_H_
#define QUEST_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

//ADCS types
#include "adcs_types.h"

//DSPLIB MATH TYPES
#include "type_defs.h"

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

//structure to contain tri-axial vector inputs for the QUEST algorithm
typedef struct
{
    float32_t mag_obs[3];
    float32_t mag_ref[3];
    float32_t sun_obs[3];
    float32_t sun_ref[3];
} adcs_ad_triax_vectors_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

adcs_err_t quest_estimate(const adcs_ad_triax_vectors_t *ad_vectors, float32_t* weights, float32_t *out_quat);

#endif // QUEST_H_
