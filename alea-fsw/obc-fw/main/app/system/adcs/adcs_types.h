/**
 * @file adcs_types.h
 * @brief Common type definitions for ADCS subsystem
 */

#ifndef ADCS_TYPES_H_
#define ADCS_TYPES_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// Public defines that may be used by other files

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Return value for ADCS functions
 */
typedef enum {
    /**
     * @brief ADCS function succeeded
     */
    ADCS_SUCCESS = 0,

    /**
     * @brief An error occured when trying to read from EPS sensors
     */
    ADCS_ERROR_EPS_SENS_READ = -1,

    /**
     * @brief ADCS calculation (math) error
     */
    ADCS_MATH_ERROR = -2,

    /**
     * @brief ADCS WMM not initialized error
     */
    ADCS_ERROR_WMM_NO_INIT = -3
} adcs_err_t;

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

// Public global variables that may be used by other files
// (always declare extern)

#endif // ADCS_TYPES_H_
