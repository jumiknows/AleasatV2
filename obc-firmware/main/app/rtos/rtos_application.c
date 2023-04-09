/**
 * @file rtos_application.c
 * @brief Implementations of application-specific FreeRTOS functions (those prefixed with "Application")
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_featuredefs.h"
#include "low_power.h"
#include "logger.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <string.h>

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief This function is called when a stack overflow event is detected by FreeRTOS.
 * 
 * See https://www.freertos.org/Stacks-and-stack-overflow-checking.html for more information.
 * 
 * @param[in] xTask      Handle of the task whose stack overflowed
 * @param[in] pcTaskName Name of the task whose stack overflowed
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    log_signal_with_data(ERROR, LOG_TASK_OVERFLOW, LOG_TASK_OVERFLOW__OVERFLOW, strlen(pcTaskName), pcTaskName);
    taskDISABLE_INTERRUPTS();
    while (1) {};
}

/**
 * @brief This function is called repeatedly by the idle task (lowest priority task) when it is allowed to run.
 * 
 * DO NOT call any API functions that could block the current task.
 * 
 * See https://www.freertos.org/a00016.html for more information.
 */
void vApplicationIdleHook(void) {
#if (OBC_IDLE_SLEEP_EN == 1)
    enter_idle_sleep();
#endif
}

/**
 * @brief This function provides pointers to the statically allocated memory for the idle task.
 * 
 * See https://www.freertos.org/a00110.html#configSUPPORT_STATIC_ALLOCATION for more information.
 * 
 * @param[out] ppxIdleTaskTCBBuffer   Pointer to where the pointer to the idle task TCB is allocated
 * @param[out] ppxIdleTaskStackBuffer Pointer to where the pointer to the idle task stack is allocated
 * @param[out] pulIdleTaskStackSize   Pointer to where the size of the idle task will be written
 */
void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer, StackType_t** ppxIdleTaskStackBuffer, uint32_t* pulIdleTaskStackSize) {
    // If the buffers to be provided to the Idle task are declared inside this
    // function then they must be declared static - otherwise they will be allocated on
    // the stack and so not exists after this function exits.
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    // Pass out a pointer to the StaticTask_t structure in which the Idle task's
    // state will be stored.
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    // Pass out the array that will be used as the Idle task's stack.
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    // Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    // Note that, as the array is necessarily of type StackType_t,
    // configMINIMAL_STACK_SIZE is specified in words, not bytes.
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/**
 * @brief This function provides pointers to the statically allocated memory for the timer service task.
 * 
 * See https://www.freertos.org/a00110.html#configSUPPORT_STATIC_ALLOCATION for more information.
 * 
 * @param[out] ppxTimerTaskTCBBuffer   Pointer to where the pointer to the timer task TCB is allocated
 * @param[out] ppxTimerTaskStackBuffer Pointer to where the pointer to the timer task stack is allocated
 * @param[out] pulTimerTaskStackSize   Pointer to where the size of the timer task will be written
 */
void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer, StackType_t** ppxTimerTaskStackBuffer, uint32_t* pulTimerTaskStackSize) {
    // If the buffers to be provided to the Timer task are declared inside this
    // function then they must be declared static - otherwise they will be allocated on
    // the stack and so not exists after this function exits.
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    // Pass out a pointer to the StaticTask_t structure in which the Timer
    // task's state will be stored.
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    // Pass out the array that will be used as the Timer task's stack.
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    // Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    // Note that, as the array is necessarily of type StackType_t,
    // configTIMER_TASK_STACK_DEPTH is specified in words, not bytes.
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
