/*
 * @file application_defined_privileged_functions.h
 * @date April 21, 2020
 * @author Richard A
 * @brief Functions that need to execute in privileged mode.
 *
 * Note: this header is a bit weird, because it includes the definitions of functions that need
 * to execute in privileged mode. It should never be included directly in user code. Instead,
 * to use functions in this file, users should #include "FreeRTOS.h" and
 * #include "rtos_mpu_wrappers.h", and declare the functions there.
 *
 * This is the FreeRTOS way of doing things:
 * https://www.freertos.org/a00110.html#configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS
 *
 * Functions defined here have access to the functions (including private ones) in @ref
 * rtos_mpu_wrappers.c, such as vPortResetPrivilege() and xPortRaisePrivilege();
 *
 */

#ifndef APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS_H_
#define APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS_H_

#include "reg_system.h"
#include "rtos_mpu_wrappers.h"

/**
 * @brief Soft resets the TMS570 immediately.
 *
 * This function raises the privilege level and then sets the soft reset
 * bit in the System Exception Control Register (SYSECR).
 *
 * Note: since the TMS570 resets, there is no need to lower the privilege level
 * back down again, we won't ever get around to that instruction. Usually, when
 * bumping up the privilege, you want to bring it back down when you're done. So
 * don't use this function as an example of what to do in a general case!
 */
void tms570_nrst(void) {
    xPortRaisePrivilege();
    systemREG1->SYSECR = 0x00008000; /* Set bit 15 of SYSECR to reset */
}

#endif /* APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS_H_ */
