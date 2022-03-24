# FreeRTOS Port for TMS570 with MPU Support

## Modifications to FreeRTOS Kernel

In order to support the TMS570 with MPU wrappers, the following modifications to the official kernel sources are required (in addition to using this port):
* `mpu_wrappers.h`:
  * `xPortRaisePrivilege` must be redefined as:
    ```c
    #define xPortRaisePrivilege( xRunningPrivileged )                      \
    {                                                                      \
        /* If the processor is not already privileged, raise privilege. */ \
        xRunningPrivileged = portRAISE_PRIVILEGE();                        \
    }
    ```
    