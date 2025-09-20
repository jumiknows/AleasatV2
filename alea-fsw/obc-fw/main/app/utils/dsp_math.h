/**
 * @file dsp_math.h
 * @brief Header wrapper for DSPLIB math
 */

#ifndef DSP_MATH_H_
#define DSP_MATH_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// DSPLIB performs comparisons between signed and unsigned operands in functions
// defined in the arm_math.h header file. As far as we know, these don't result
// in any actual bugs, so we will suppress the diagnostic for it when using
// arm_math.h

#pragma diag_push
#pragma diag_suppress 2142 // comparison between signed and unsigned operands

#include "arm_math.h"

#pragma diag_pop

#endif // DSP_MATH_H_
