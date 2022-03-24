/**
 * @file blinky.c
 * @brief Tasks that blink LEDs.
 * @author Richard Arthurs
 * @date October 7, 2018
 */

#include "obc_watchdog.h"
#include "obc_gpio.h"
#include "rtos.h"
#include "blinky.h"
#include "obc_hardwaredefs.h"
#include "obc_featuredefs.h"
#include "obc_task_info.h"
#include "obc_rtos.h"

// Variables
TaskHandle_t xHerculesBlinkyTaskHandle = NULL;
TaskHandle_t xExpanderBlinkyTaskHandle = NULL;

// Private Functions
void hercules_blinky(void* pvParameters);
void expander_blinky(void* pvParameters);

/**
 * @brief Starts the built-in GPIO blinky task. This task blinks an LED
 * attached to a GPIO pin at 5 Hz.
 */
void blinky_start_task(void) {
    static StaticTask_t blinky_task_buf                          = {NULL};
    static StackType_t blinky_task_stack[BLINKY_TASK_STACK_SIZE] = {NULL};
    xHerculesBlinkyTaskHandle = task_create_periodic_static(&hercules_blinky, "blinky", BLINKY_TASK_STACK_SIZE, NULL, BLINKY_TASK_DEFAULT_PRIORITY, blinky_task_stack, &blinky_task_buf, 200);
}

/**
 * @brief Starts the blinky task that uses a GPIO expander pin. This task
 * blinks an LED at 1 Hz and demonstrates continuous use of the GPIO expander.
 *
 * This will only create the task on platforms that have a GPIO expander.
 */
void expander_blinky_start_task(void) {
#if FEATURE_GPIO_EXPANDER
    static StaticTask_t blinky_task_buf                                   = {NULL};
    static StackType_t blinky_task_stack[EXPANDER_BLINKY_TASK_STACK_SIZE] = {NULL};
    xExpanderBlinkyTaskHandle =
        task_create_periodic_static(&expander_blinky, "exp_blinky", EXPANDER_BLINKY_TASK_STACK_SIZE, NULL, BLINKY_TASK_DEFAULT_PRIORITY, blinky_task_stack, &blinky_task_buf, 200);
#endif
}

/**
 * @brief Blinks an LED using a GPIO pin at 5 Hz.
 * This is an example of a very basic task.
 */
void hercules_blinky(void* pvParameters) {
    uint32_t blink       = 0;
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);

    while (1) {
        set_task_status(wd_task_id, task_alive);

        obc_gpio_read(OBC_BLINKY_PORT, OBC_BLINKY_PIN, &blink);
        obc_gpio_write(OBC_BLINKY_PORT, OBC_BLINKY_PIN, blink ^ 1);

        set_task_status(wd_task_id, task_asleep);
        vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD_MS(pvParameters)));
    }
}

/**
 * @brief Blinks an LED using a GPIO expander pin at 5 Hz.
 */
void expander_blinky(void* pvParameters) {
#if FEATURE_GPIO_EXPANDER

    uint32_t blink       = 0;
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);

    while (1) {
        set_task_status(wd_task_id, task_alive);

        obc_gpio_read(EXPANDER_BLINKY_PORT, EXPANDER_BLINKY_PIN, &blink);
        obc_gpio_write(EXPANDER_BLINKY_PORT, EXPANDER_BLINKY_PIN, blink ^ 1);

        set_task_status(wd_task_id, task_asleep);

        vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD_MS(pvParameters)));
    }
#endif
}
