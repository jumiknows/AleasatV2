/**
 * @file application_defined_privileged_functions.h
 * @brief Functions that need to execute in privileged mode.
 *
 * This is not a typical header file. It is included by FreeRTOS inside of mpu_wrappers.c and
 * should not be used anywhere else.
 *
 * This file can contain function *definitions* (not just declarations) that need to have access
 * to xPortRaisePrivilege() and vPortResetPrivilege() so they can run code in privileged mode.
 *
 * More information:
 * https://www.freertos.org/a00110.html#configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS
 */

#ifndef APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS_H_
#define APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS_H_

/**
 * @brief Performs an immediate software reset
 */
void vPrivilegedCPUReset(void) {
    // Raise privilege
    BaseType_t xRunningPrivileged;
    xPortRaisePrivilege( xRunningPrivileged );

    // Privileged code
    portRESET_CPU();

    // Reset privilege
    vPortResetPrivilege( xRunningPrivileged ); // shouldn't ever actually reach here because we're performing a reset
}

#endif /* APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS_H_ */
