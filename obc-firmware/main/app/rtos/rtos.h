/**
 * @file rtos.h
 * @brief Header file to include all FreeRTOS dependencies
 */

#ifndef RTOS_H_
#define RTOS_H_

#include "obc_misra.h"

OBC_MISRA_CHECK_OFF

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"
#include "timers.h"
#include "stream_buffer.h"
#include "message_buffer.h"

OBC_MISRA_CHECK_ON

#endif // RTOS_H_
