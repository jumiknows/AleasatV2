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

Since the placement of the kernel code is controlled by the linker script, the MPU initialization function must be modified to use an address supplied through a linker-defined symbol for the start of the kernel region:
* `port.c`
  * The following global must be defined:
    ```c
    /**
    * @brief Linker-defined symbol whose address is the start of the kernel code.
    * 
    * This address will be aligned to 32 KB before configuring the MPU region, which
    * means it will absorb the FW structs that precede it in the flash. This is why
    * an additional MPU region must be configured to restore user read access to the
    * FW structs.
    */
    extern void * KERNEL_START;
    ```
  * The configuration of the kernel region in `prvSetupDefaultMPU` must be changed to:
    ```c
    prvMpuSetRegion(portPRIVILEGED_FLASH_REGION,  (((uintptr_t)&KERNEL_START) & kernel_mask), portMPU_SIZE_32KB | portMPU_REGION_ENABLE, portMPU_PRIV_RO_USER_NA_EXEC | portMPU_NORMAL_OIWTNOWA_SHARED);
    ```

In order to allow user-mode code to access the FW structs stored at the beginning of the firmware image, an additional MPU region must be added to the default configuration:
* `portmacro.h`:
  * The following definitions must be modified / added:
    ```c
    #define portLAST_CONFIGURABLE_REGION            ( portMPU_TOTAL_REGIONS - 3 )
    #define portUNPRIVILEGED_FW_STRUCTS_REGION      ( portMPU_TOTAL_REGIONS - 2 )
    ```
* `portASM.asm`:
  * The number of configurable MPU regions is effectively hard-coded into the `portRESTORE_CONTEXT` assembly macro (through `portMax_MPU_Region`), so we need to update this field to reflect the fact that there is one fewer configurable MPU region by changing it to:
    ```c
    portMax_MPU_Region       .word   12 - 2
    ```
* `port.c`
  * The following global must be defined:
    ```c
    /**
    * @brief Linker-defined symbol whose address is the start of the FW header
    * (which coincides with the start of the FW structs).
    */
    extern void * FW_HEADER;
    ```
  * The following must be added to the `prvSetupDefaultMPU` function:
    ```c
    /* Unprivileged FW structs region setup */
    uint32_t fw_structs_mask = ~(0x40 - 1); // align to 64 bytes
    prvMpuSetRegion(portUNPRIVILEGED_FW_STRUCTS_REGION,  (((uintptr_t)&FW_HEADER) & fw_structs_mask), portMPU_SIZE_64B | portMPU_REGION_ENABLE, portMPU_PRIV_RO_USER_RO_EXEC | portMPU_NORMAL_OIWTNOWA_SHARED);
    ```
