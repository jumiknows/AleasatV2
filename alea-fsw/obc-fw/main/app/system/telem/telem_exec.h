/**
 * @file telem_exec.h
 * @brief Telemetry executor
 */

#ifndef TELEM_EXEC_H_
#define TELEM_EXEC_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "telem.h"
#include "telem_error.h"
#include "telem_gen.h"
#include "rtos.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void telem_exec_pre_init(void);

void telem_exec_enqueue(const uint16_t telem_unit_id);

#endif // TELEM_EXEC_H_
