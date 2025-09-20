/**
 * @file obc_watchdog.c
 * @brief OBC software watchdog implementation
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_watchdog.h"

// Drivers
#if FEATURE_HW_WATCHDOG
#include "obc_gpio.h"
#endif

// OBC
#include "obc_featuredefs.h"
#include "obc_hardwaredefs.h"
#include "obc_rtos.h"
#include "obc_tasks_gen.h"
#include "obc_utils.h"

// Logging
#include "logger.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define WATCHDOG_TASK_PERIOD_MS 2000U

#define OBC_TASK_ID_TO_BIT_MASK(_id) ((uint32_t)(1U << (_id)))

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void obc_watchdog_task(void *pvParameters);
static void bite_task(obc_task_id_t task_id);

#if FEATURE_HW_WATCHDOG
static void pet_hw_watchdog(void);
#endif

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static EventGroupHandle_t wd_event_group_handle;
static uint32_t watched_tasks_mask = OBC_TASK_ID_TO_BIT_MASK(OBC_TASK_ID_IDLE); // Always watch the IDLE task
static obc_watchdog_action_t wd_actions[OBC_TASK_COUNT] = {};
static bool wd_task_started = false;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize FreeRTOS objects used by this module
 */
void obc_watchdog_pre_init(void) {
    static StaticEventGroup_t event_group_buffer;
    wd_event_group_handle = xEventGroupCreateStatic(&event_group_buffer);
}

/**
 * @brief Start the software watchdog task
 */
void obc_watchdog_post_init(void) {
    obc_rtos_create_task(OBC_TASK_ID_WATCHDOG, &obc_watchdog_task, NULL, OBC_WATCHDOG_ACTION_IGNORE);
    wd_task_started = true;
}

/**
 * @brief Register a watchdog action for a particular task
 *
 * @warning This function is NOT thread-safe. It may only be called from a single thread.
 * @warning This function CANNOT be called after obc_watchdog_start_task().
 *
 * @param task_id ID of the task
 * @param action  Action for the watchdog to take when the task stops responding
 */
void obc_watchdog_watch_task(obc_task_id_t task_id, obc_watchdog_action_t action) {
    RUNTIME_ASSERT(!wd_task_started);

    if (action == OBC_WATCHDOG_ACTION_IGNORE) {
        // Clear task bit
        watched_tasks_mask &= ~(OBC_TASK_ID_TO_BIT_MASK(task_id));
    } else {
        // Set task bit
        watched_tasks_mask |= OBC_TASK_ID_TO_BIT_MASK(task_id);
    }

    wd_actions[task_id] = action;
}

/**
 * @brief Pet the software watchdog (informing it that the specified task is still responding)
 *
 * @param task_id ID of the task that is petting
 */
void obc_watchdog_pet(obc_task_id_t task_id) {
    xEventGroupSetBits(wd_event_group_handle, OBC_TASK_ID_TO_BIT_MASK(task_id));
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

static void obc_watchdog_task(void *pvParameters) {
    TickType_t last_wake_time = xTaskGetTickCount();

    while (1) {
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(WATCHDOG_TASK_PERIOD_MS));

#if FEATURE_HW_WATCHDOG
        LOG_WATCHDOG__HW_WD_PET();
        pet_hw_watchdog();
#endif

        if (watched_tasks_mask != 0) {
            EventBits_t set_bits =  xEventGroupWaitBits(
                                        wd_event_group_handle,
                                        watched_tasks_mask,
                                        pdTRUE,  // ClearOnExit
                                        pdFALSE, // WaitForAllBits - since the timeout is zero, this must be false otherwise the bits won't be cleared
                                        0        // TicksToWait
                                    );

            // Keep only the bits corresponding to tasks we are watching AND didn't pet the watchdog
            // These are the tasks the watchdog must bite
            uint32_t bite_mask = (~set_bits) & watched_tasks_mask;

            if (bite_mask == 0) {
                // All the tasks pet the watchdog, so the watchdog is happy
                LOG_WATCHDOG__SW_WD_HAPPY();
            } else {
                // Some of the tasks didn't pet the watchdog and will be bitten
                LOG_WATCHDOG__SW_WD_BITE(bite_mask);

                // Bite each task
                uint8_t task_id = 0;

                while ((bite_mask != 0) && (task_id < OBC_TASK_COUNT)) {
                    if (bite_mask & 0x1) {
                        bite_task((obc_task_id_t)task_id);
                    }

                    bite_mask >>= 1;
                    task_id++;
                }
            }
        }
    }
}

static void bite_task(obc_task_id_t task_id) {
    if (task_id >= OBC_TASK_COUNT) {
        return;
    }

    switch (wd_actions[task_id]) {
    case OBC_WATCHDOG_ACTION_IGNORE:
    case OBC_WATCHDOG_ACTION_ALLOW:
        break;

    case OBC_WATCHDOG_ACTION_SOFT_RESET:
        // TODO: delay this until the IDLE task (up to some maximum number of delays)
        CPU_SOFT_RESET();
        break;

    default:
        break;
    }
}

#if FEATURE_HW_WATCHDOG
static void pet_hw_watchdog(void) {
    obc_gpio_write(OBC_WD_PORT, OBC_WD_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(WD_PET_PULSE_MS));
    obc_gpio_write(OBC_WD_PORT, OBC_WD_PIN, 0);
}
#endif
