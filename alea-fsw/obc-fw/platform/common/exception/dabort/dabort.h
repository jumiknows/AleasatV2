/**
 * @file dabort.h
 * @brief Helper functions to manage data aborts
 */

#ifndef DABORT_H_
#define DABORT_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void dabort_FUNC_ERR_set_nonfatal(void);
void dabort_FUNC_ERR_set_fatal(void);

bool dabort_FUNC_ERR_get(void);
uint32_t dabort_FUNC_ERR_clear(void);

#endif // DABORT_H_
