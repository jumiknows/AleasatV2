/*
 * gpio_interrupt_example.c
 *
 *  Created on: Feb 19, 2018
 *      Author: Richard
 *
 *      This example will demonstrate GPIO interrupt functionality and how to integrate the RTOS
 * with it. Main reference: FreeRTOS Tutorial Guide Chapter 6.4 - Binary Semaphores
 */

#include "orcasat_examples.h"
#include "obc_hardwaredefs.h"
#include "obc_featuredefs.h"
#include "sys_common.h"
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_semphr.h"
#include "obc_uart.h"
#include "gio.h"
#include "het.h"
#include "logger.h"

/* HALCoGen Setup
 * 		- TMS570LS0714PGE > Driver Enable > Enable GIO Driver
 * 		- Pinmux > Enable the pin
 * 		- GIO > Port B > Enable VIM high priority and rising edge on whichever pin you want
 * 		- TMS570LS0714PGE > VIM Channel 0-31 > Enable channel 9
 */

/* USING THIS EXAMPLE
 * - In obc_notification.c, call gio_notification_example() in gioNotification()
 * - Include examples/orcasat_examples.h
 *
 * - In sys_main:
 * 		- Enable interrupts in sys_main.c [_enable_IRQ(); and _enable_interrupt_();] - this is
 * probably done already
 * 		- Call het_interrupt_example_rtos_init();
 * 		- Start the RTOS scheduler
 *
 * - On the rising edge, the task will fire and print something out over the UART. The blinky LED
 * will also toggle.
 */
#if GIO_EXAMPLE_EN

SemaphoreHandle_t gioExampleSem;
TaskHandle_t xGioInterruptTaskHandle;

void het_interrupt_example_rtos_init(void) {
    // Standalone setup of the task and semaphore so the example can be run easily.
    // Create the semaphore and the task handle. Task handle creation normally in task_main.c
    gioExampleSem           = xSemaphoreCreateBinary();
    xGioInterruptTaskHandle = NULL;

    if (gioExampleSem != NULL) { // setup the task to handle the ISR
        xTaskCreate(&vGioInterruptTask, "GIO Int", 200, NULL, 3, xGioInterruptTaskHandle);
    }

    hetInit();
    edgeEnableNotification(SEMAPHORE_IRQ_REG,
                           SEMAPHORE_IRQ_EDGE); // enables notification on USER SWITCH B for the launchpad
}

void vGioInterruptTask(void* pvParameters) {
    // This task gets invoked by the ISR callback (notification) whenever the pin sees a rising edge
    while (1) {
        xSemaphoreTake(gioExampleSem, portMAX_DELAY);
        log_str(DEBUG, PRINT_GENERAL, false, "ISR Fired!");
        gioSetBit(IRQ_EXAMPLE_LED_PORT, IRQ_EXAMPLE_LED_PIN, gioGetBit(IRQ_EXAMPLE_LED_PORT, IRQ_EXAMPLE_LED_PIN) ^ 1); // Toggles the blinky LED
    }
}

void het_notification_example(hetBASE_t* hetREG, uint32_t edge) {
    // This gets called by the ISR callback (notification). Doesn't need to be in another function,
    // but this just keeps things tidy for the example See FreeRTOS tutorial guide pg. 200

    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    if ((hetREG == SEMAPHORE_IRQ_REG) && (edge == SEMAPHORE_IRQ_EDGE)) { // Always need to check which pin actually triggered the interrupt

        xSemaphoreGiveFromISR(gioExampleSem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
#endif
