/**
 * @file blinky.c
 * @brief Blinky task
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "blinky.h"

// OBC
#include "obc_hardwaredefs.h"
#include "obc_featuredefs.h"
#include "obc_watchdog.h"
#include "obc_rtos.h"
#include "obc_gpio.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define BLINKY_PERIOD_MS 200U

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void blinky_task(void *pvParameters);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Starts the built-in GPIO blinky task. This task blinks an LED
 * attached to a GPIO pin at 5 Hz.
 */
void blinky_pre_init(void) {
    obc_rtos_create_task(OBC_TASK_ID_BLINKY, &blinky_task, NULL, OBC_WATCHDOG_ACTION_ALLOW);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Blinks an LED using a GPIO pin at 5 Hz.
 * This is an example of a very basic task.
 */
static void blinky_task(void *pvParameters) {
    uint32_t blink = 0;

    TickType_t last_wake_time = xTaskGetTickCount();

    while (1) {
        obc_watchdog_pet(OBC_TASK_ID_BLINKY);

        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(BLINKY_PERIOD_MS / 2));

        obc_gpio_read(OBC_BLINKY_PORT, OBC_BLINKY_PIN, &blink);
        obc_gpio_write(OBC_BLINKY_PORT, OBC_BLINKY_PIN, blink ^ 1);

        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(BLINKY_PERIOD_MS / 2));

#if FEATURE_GPIO_EXPANDER
        obc_gpio_read(EXPANDER_BLINKY_PORT, EXPANDER_BLINKY_PIN, &blink);
        obc_gpio_write(EXPANDER_BLINKY_PORT, EXPANDER_BLINKY_PIN, blink ^ 1);
#endif
    }
}
