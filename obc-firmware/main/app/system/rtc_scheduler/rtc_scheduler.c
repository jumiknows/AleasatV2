/**
 * @file rtc_scheduler.c
 * @brief RTC-based scheduler
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "rtc_scheduler.h"

// OBC
#include "obc_rtc.h"

// Utils
#include "sequence.h"
#include "obc_utils.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define SCHEDULE_MAX_ITEMS 8U

#define ALARM_CALLBACK_MUTEX_TIMEOUT_MS 100

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void alarm_callback(void *arg);
static void update_alarm(void);

static void store_item(const void *item, uint8_t storage_idx);
static bool goes_before(uint8_t idx_a, uint8_t idx_b);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static rtc_scheduler_item_t scheduler_storage[SCHEDULE_MAX_ITEMS] = { 0 };
static uint8_t scheduler_data_storage[SCHEDULE_MAX_ITEMS][RTC_SCHEDULER_MAX_DATA_SIZE] = { 0 };

static uint8_t sequence_links[SCHEDULE_MAX_ITEMS] = { 0 };
static sequence_t scheduler_sequence = {
    .size             = SCHEDULE_MAX_ITEMS,

    .store_item       = &store_item,
    .goes_before      = &goes_before,

    .links            = sequence_links,
    .active_list_head = 0,
    .empty_list_head  = 0,
};

static SemaphoreHandle_t schedule_mutex = NULL;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Create FreeRTOS infrastructure needed by this module
 */
void rtc_scheduler_create_infra(void) {
    static StaticSemaphore_t schedule_mutex_buf = { 0 };

    uint32_t i = 0;

    // Clear all the schedule data
    memset(scheduler_storage, 0, sizeof(scheduler_storage));
    memset(scheduler_data_storage, 0, sizeof(scheduler_data_storage));
    for (i = 0; i < LEN(scheduler_storage); i++) {
        scheduler_storage[i].timestamp = 0;
        scheduler_storage[i].priority = 0;
        scheduler_storage[i].cb = NULL;
        scheduler_storage[i].data_len = 0;
        scheduler_storage[i].data = &(scheduler_data_storage[i][0]);
    }

    // Initialize sequence
    sequence_init(&scheduler_sequence);

    schedule_mutex = xSemaphoreCreateMutexStatic(&schedule_mutex_buf);
}

/**
 * @brief Add an item to the schedule and update the RTC alarm
 * 
 * @param[in] item          Pointer to the item to schedule
 * @param[in] timeout_ticks Timeout for mutex (ticks)
 * 
 * @return Status code:
 *            - RTC_SCHEDULER_SUCCESS if the item is successfully scheduled
 *            - RTC_SCHEDULER_ERR_INVALID_ARGS if item is NULL
 *            - RTC_SCHEDULER_ERR_DATA_TOO_LONG if the item data_len is too long (see RTC_SCHEDULER_MAX_DATA_SIZE)
 *            - RTC_SCHEDULER_FULL if there is no more space left in the schedule memory
 *            - RTC_SCHEDULER_MUTEX_TIMEOUT if a timeout occurs waiting for the schedule mutex
 */
rtc_scheduler_err_t rtc_scheduler_add_item(const rtc_scheduler_item_t *item, uint32_t timeout_ticks) {
    // Check arguments
    if (item == NULL) {
        return RTC_SCHEDULER_ERR_INVALID_ARGS;
    }
    if (item->data_len > RTC_SCHEDULER_MAX_DATA_SIZE) {
        return RTC_SCHEDULER_ERR_DATA_TOO_LONG;
    }

    // Acquire mutex
    if (xSemaphoreTake(schedule_mutex, timeout_ticks) == pdFALSE) {
        return RTC_SCHEDULER_MUTEX_TIMEOUT;
    }

    rtc_scheduler_err_t err = RTC_SCHEDULER_FULL;
    if (sequence_insert(&scheduler_sequence, item)) {
        // Item successfully inserted into schedule
        update_alarm();
        err = RTC_SCHEDULER_SUCCESS;
    }

    // Release mutex
    xSemaphoreGive(schedule_mutex);

    return err;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Callback triggered by the RTC alarm
 * 
 * @param[in] arg Index of the item that triggered the alarm
 */
static void alarm_callback(void *arg) {
    uint32_t idx = (uint32_t)arg;
    if (idx < SCHEDULE_MAX_ITEMS) {
        if (xSemaphoreTake(schedule_mutex, pdMS_TO_TICKS(ALARM_CALLBACK_MUTEX_TIMEOUT_MS)) == pdFALSE) {
            return;
        }

        const rtc_scheduler_item_t *item = &scheduler_storage[idx];

        item->cb(item->data, item->data_len);

        sequence_pop_first(&scheduler_sequence);
        update_alarm();

        xSemaphoreGive(schedule_mutex);
    }
}

/**
 * @brief Set the RTC alarm for the soonest scheduled item
 */
static void update_alarm(void) {
    if (sequence_is_empty(&scheduler_sequence)) {
        return;
    }

    uint8_t idx = sequence_peek_first(&scheduler_sequence);
    const rtc_scheduler_item_t *next_item = &scheduler_storage[idx];
    rtc_set_absolute_alarm(next_item->timestamp, &alarm_callback, (void *)idx);
}

/**
 * @brief Store an item at a particular index in the scheduler_storage
 * 
 * @param[in] item        Pointer to the item to store
 * @param[in] storage_idx Index where the item should be stored
 */
static void store_item(const void *item, uint8_t storage_idx) {
    const rtc_scheduler_item_t *src_item = (const rtc_scheduler_item_t *)item;
    rtc_scheduler_item_t *dest_item = &scheduler_storage[storage_idx];

    // Copy data into storage
    dest_item->timestamp = src_item->timestamp;
    dest_item->priority  = src_item->priority;
    dest_item->cb        = src_item->cb;
    dest_item->data_len  = src_item->data_len;
    dest_item->data      = NULL;

    if (src_item->data_len > 0) {
        dest_item->data = &(scheduler_data_storage[storage_idx][0]);
        memcpy(dest_item->data, src_item->data, src_item->data_len);
    }
}

/**
 * @brief Evaluate if the item at idx_a should occur before the item at idx_b in the schedule.
 * 
 * If idx_a has an earlier timestamp OR they have the same timestamp but idx_a has a higher priority,
 * then idx_a will be placed earlier in the schedule.
 * 
 * @param[in] idx_a Index of the item being placed
 * @param[in] idx_b Index of the item to compare to
 * 
 * @return true if the item at idx_a should occur before the item at idx_b, otherwise false
 */
static bool goes_before(uint8_t idx_a, uint8_t idx_b) {
    if (scheduler_storage[idx_a].timestamp < scheduler_storage[idx_b].timestamp) {
        return true;
    } else if (scheduler_storage[idx_a].timestamp == scheduler_storage[idx_b].timestamp) {
        if (scheduler_storage[idx_a].priority > scheduler_storage[idx_b].priority) {
            return true;
        }
    }
    return false;
}
