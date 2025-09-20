/**
 * @file rtc_scheduler.h
 * @brief RTC-based scheduler
 */

#ifndef RTC_SCHEDULER_H_
#define RTC_SCHEDULER_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Logger
#include "logger.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// For now this is matched to the maximum logger MAX_PAYLOAD_SIZE because the
// scheduled command responses are sent out in single log packets.
// TODO: ALEA-858 choose an independent value for this that makes sense.
#define RTC_SCHEDULER_MAX_DATA_SIZE  MAX_PAYLOAD_SIZE

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
    RTC_SCHEDULER_SUCCESS           = 0,
    RTC_SCHEDULER_ERR_INVALID_ARGS  = 1,
    RTC_SCHEDULER_ERR_DATA_TOO_LONG = 2,
    RTC_SCHEDULER_FULL              = 3,
    RTC_SCHEDULER_MUTEX_TIMEOUT     = 4
} rtc_scheduler_err_t;

typedef void (*rtc_scheduler_cb_t)(uint8_t *data, uint32_t data_len);

typedef struct {
    uint32_t timestamp; // used as marker field to indicate valid data (!= 0)
    uint8_t priority;
    rtc_scheduler_cb_t cb;
    uint32_t data_len;
    uint8_t *data;
} rtc_scheduler_item_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void rtc_scheduler_create_infra(void);

rtc_scheduler_err_t rtc_scheduler_add_item(const rtc_scheduler_item_t *item, uint32_t timeout_ticks);

#endif // RTC_SCHEDULER_H_
