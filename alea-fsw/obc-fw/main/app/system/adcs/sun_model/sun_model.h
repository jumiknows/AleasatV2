/**
 * @file sun_model.h
 * @brief Implementation of sun model which returns the position of the sun in GCRS frame.
 * The code is ported from SOFA http://www.iausofa.org.
 */

#ifndef SUN_MODEL_H
#define SUN_MODEL_H

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Utils
#include "obc_utils.h"

// DSPLIB
#include "dsp_math.h"

// Standard Library
#include <string.h>

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void sun_model_get_pos(const int16_t *time, float32_t *jul, float32_t *sun_pos);

#endif // SUN_MODEL_H
