/*
 * obc_stack_overflow.c
 *
 *  Created on: Apr 29, 2019
 *      Author: Richard
 */

/* Stack Overflow Hook
 * 	- FreeRTOS will call this if it detects a stack overflow
 */
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "obc_uart.h"
#include "logger.h"

void vApplicationStackOverflowHook(TaskHandle_t pxTask, signed char* pcTaskName);

void vApplicationStackOverflowHook(TaskHandle_t pxTask, signed char* pcTaskName) {
    (void)pcTaskName;
    (void)pxTask;

    log_str(ERROR, TASK_OVERFLOW, true, "%s has overflowed", (char*)pcTaskName);

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    while (1) {
    };
}
