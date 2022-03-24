/*******************************************************
 * rtos_customization.c
 *
 * Created on: May 20, 2019
 *  Author(s): Alireza.A
 *
 *******************************************************/

#ifndef RTOS_CUSTOMIZATION_H_
#define RTOS_CUSTOMIZATION_H_

#include "rtos.h"

void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer, StackType_t** ppxIdleTaskStackBuffer, uint32_t* pulIdleTaskStackSize);
void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer, StackType_t** ppxTimerTaskStackBuffer, uint32_t* pulTimerTaskStackSize);

#endif // RTOS_CUSTOMIZATION_H_
