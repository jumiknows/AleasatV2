/*
 * orcasat_examples.h
 *
 *  Created on: Feb 19, 2018
 *      Author: Richard
 */

#ifndef ORCASAT_EXAMPLES_H_
#define ORCASAT_EXAMPLES_H_

#include "FreeRTOS.h"
#include "rtos_task.h"
#include "rtos_queue.h"
#include "rtos_semphr.h"
#include "rtos_portmacro.h"
#include "het.h"

// HET Interrupt Example
extern SemaphoreHandle_t gioExampleSem;
extern TaskHandle_t xGioInterruptTaskHandle;

void het_interrupt_example_rtos_init(void);                      // standalone initializer for tasks and semaphore
void het_notification_example(hetBASE_t* hetREG, uint32_t edge); // called in edgeNotification, raises semaphore to start the task
void vGioInterruptTask(void* pvParameters);                      // Task that runs when the interrupt fires

#endif /* ORCASAT_EXAMPLES_H_ */
