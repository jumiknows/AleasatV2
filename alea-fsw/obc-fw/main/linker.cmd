/**
 * @file linker.cmd
 * @brief Linker Command File for all platforms
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "fw_memmap.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define RAM_TOTAL_SIZE  0x20000

/******************************************************************************/
/*                         C O N F I G U R A T I O N                          */
/******************************************************************************/

#if (CFG_STANDALONE == 1)
--retain="platform/libplatform.a<*>(.exc_flash)"
#endif

--retain="platform/libplatform.a<*>(.exc_ram)"

/*
 * This retain section is not explicitly needed, but included for developer convenience.
 * While debugging/testing, if any of the following symbols are optimized away by the compiler, there would
 * be a link time error for the .F021API section.
 */
#if (CFG_USE_F021API == 1)
--retain="lib/f021-api/libF021-API.a<*>(.text)"
--retain="../../../../lib/f021-api/lib/F021_API_CortexR4_BE.lib<*>(.text)"
--retain="boot_fapi.c.obj (.text)"
#endif

/******************************************************************************/
/*                                M E M O R Y                                 */
/******************************************************************************/

MEMORY
{
#if (CFG_STANDALONE == 1)
    EXCFLASH   : origin=0x00000000                  length=FW_MEMMAP_EXCFLASH_SIZE
#endif

    /*
     * WARNING: The FW_HEADER must start at a 32 KB (0x8000) aligned address if an RTOS is used in this firmware image.
     *          This ensures the FW_HEADER up to KERNEL regions all fit within one 32 KB aligned region.
     *
     * This effectively means that the firmware image at the beginning of the flash space (immediately following the
     * EXCFLASH region) cannot use an RTOS.
     */
    FW_HEADER  : origin=FW_MEMMAP_FLASH_START_ADDR  length=FW_MEMMAP_HEADER_SIZE                                            fill=0xFFFFFFFF
    ENTRYPOINT : origin=END(FW_HEADER)              length=FW_MEMMAP_ENTRYPOINT_SIZE
    FW_INFO    : origin=END(ENTRYPOINT)             length=FW_MEMMAP_INFO_SIZE

#if (CFG_USE_RTOS == 1)
    /*
     * The FreeRTOS MPU configuration assumes the total size of the FW_HEADER up to KERNEL regions is 32 KB (0x8000).
     * If this changes, the MPU configuration in prvSetupDefaultMPU must be updated accordingly.
     */
    KERNEL     : origin=END(FW_INFO)                length=(0x00008000 - (END(FW_INFO) - START(FW_HEADER)))                 fill=0xFFFFFFFF

    #define USER_CODE_START END(KERNEL)
#else
    #define USER_CODE_START END(FW_INFO)
#endif

    USER_CODE  : origin=USER_CODE_START             length=(FW_MEMMAP_FLASH_SIZE - (USER_CODE_START - START(FW_HEADER)))

    /*STACK_SIZE is defined in CMakeLists.txt*/
    STACKS     : origin=0x08000000  length=STACK_SIZE

#if (CFG_USE_RTOS == 1)
    /*
     * The FreeRTOS MPU configuration has the total size of the STACKS and KRAM
     * memory regions hard-coded as 4 KB (0x1000). If this changes, the MPU
     * configuration in vPortStoreTaskMPUSettings must be updated accordingly.
     */
    KRAM       : origin=END(STACKS) length=0x00000800

    #define USER_RAM_START  END(KRAM)
#else
    #define USER_RAM_START  END(STACKS)
#endif

    USER_RAM   : origin=USER_RAM_START length=(RAM_TOTAL_SIZE - (USER_RAM_START - START(STACKS)) - FW_MEMMAP_EXCRAM_SIZE)

    EXCRAM     : origin=END(USER_RAM)  length=FW_MEMMAP_EXCRAM_SIZE
}

/******************************************************************************/
/*                              S E C T I O N S                               */
/******************************************************************************/

SECTIONS
{
#if (CFG_STANDALONE == 1)
    /*
     * Since EXCEPTIONS_ADDR is an address instead of a MEMORY region,
     * this section will be allocated first
     */
    .exc_flash            : {} > EXCFLASH
#endif

    .entrypoint           : {} > ENTRYPOINT
    .fwinfo               : {} > FW_INFO

#if (CFG_USE_RTOS == 1)
    /* FreeRTOS Kernel in protected region of Flash */
    privileged_functions  : {} > KERNEL,    ALIGN(8)
    .cinit                : {} > KERNEL,    ALIGN(8)
#else
    .cinit                : {} > USER_CODE, ALIGN(8)
#endif

    /* Copy tables other than boot time (.binit) copy tables. This is a read-only section. */
    .ovly                 : {} > USER_CODE, ALIGN(8)

    /* Rest of code to user mode flash region */
#if (CFG_USE_RTOS == 1)
    freertos_system_calls : {} > USER_CODE, ALIGN(8)
#endif
    .text                 : {} > USER_CODE, ALIGN(8)
    .const                : {} > USER_CODE, ALIGN(8)

    .ASM_UTILS            : {
        asm_utils.asm.obj (.text)
    } load=USER_CODE, run=USER_RAM, palign=8, LOAD_START(asm_utils_load), RUN_START(asm_utils_run), SIZE(asm_utils_size)

#if (CFG_USE_F021API == 1)
    /*
    * The F021 flash API cannot be executed on the same bank as the active bank selected for the API commands to run on
    * We need to copy the API to RAM
    *
    * NOTE: If any of the components here are removed in the future, remember to unretain them at the start of the file
    */
    .F021API               : {
        --library="lib/f021-api/libF021-API.a" (.text)
        --library="../../../../lib/f021-api/lib/F021_API_CortexR4_BE.lib" (.text)
        boot_fapi.c.obj (.text)
    } load=USER_CODE, run=USER_RAM, palign=8, LOAD_START(f021_load), RUN_START(f021_run), SIZE(f021_size)
#endif

#if (CFG_USE_RTOS == 1)
    /* FreeRTOS Kernel data in protected region of RAM */
    privileged_data       : {} > KRAM,      ALIGN(8)
    .kernelHEAP           : {} > USER_RAM,  ALIGN(8)
#endif

    .bss                  : {} > USER_RAM,  ALIGN(8)
    .data                 : {} > USER_RAM,  ALIGN(8)
    .sysmem               : {} > USER_RAM,  ALIGN(8)

    .exc_ram              : {} load=USER_CODE, run=EXCRAM, palign=8, table(EXCRAM_COPY_TABLE)
}

/******************************************************************************/
/*                               S Y M B O L S                                */
/******************************************************************************/

#if (CFG_USE_RTOS == 1)
    KERNEL_START  = START(KERNEL);
#endif

FW_HEADER     = START(FW_HEADER);
FW_ENTRYPOINT = START(ENTRYPOINT);

/* This is not actually a valid symbol, just a way of passing the magic value to entrypoint.asm */
FW_ENTRYPOINT_MAGIC_VALUE = FW_ENTRYPOINT_MAGIC;
