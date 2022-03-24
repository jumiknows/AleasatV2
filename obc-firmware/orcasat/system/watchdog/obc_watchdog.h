/*
 * obc_watchdog_pet.h
 *
 *  Created on: Aug 5, 2019
 *      Author: Lana
 */

#ifndef OBC_WATCHDOG_PET_H_
#define OBC_WATCHDOG_PET_H_

#include "rtos.h"
#include "obc_rtos.h"
#include "stdbool.h"

// Arbitrary value to wait for mutex to free up
#define WD_MUTEX_WAIT_MS 500

typedef enum { task_alive, task_unknown, task_asleep } watchdog_task_status_t;

typedef uint8_t task_id_t;

void wd_create_infra(void);
void wd_start_task(void);
void watchdog_reset(void);
void vWatchdogPetTask(void* pvParameters);
bool set_task_status(task_id_t task_id, watchdog_task_status_t status);
bool get_task_status(task_id_t task_id, watchdog_task_status_t* task_status);

void task_suspend_wd(TaskHandle_t xTaskToSuspend);
void task_resume_wd(TaskHandle_t xTaskToResume);
#endif /* OBC_WATCHDOG_PET_H_ */
