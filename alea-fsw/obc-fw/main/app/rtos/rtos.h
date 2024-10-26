/**
 * @file rtos.h
 * @brief Header file to include all FreeRTOS dependencies
 */

#ifndef RTOS_H_
#define RTOS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"
#include "timers.h"
#include "stream_buffer.h"
#include "message_buffer.h"

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// This function is defined in application_defined_privileged_functions.h
void vPrivilegedCPUReset(void);

#endif // RTOS_H_
