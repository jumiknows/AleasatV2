/**
 * @file linker.cmd
 * @brief Linker Command File for launchpad platform
 */

// Interrupt vectors are not directly referenced so would be optimized out if not explicitly retained
--retain="platform/libplatform.a<*>(.intvecs)"

MEMORY
{
    /**
     * To get the app to build correctly for the bootloader, replace first line with:
     *   VECTORS (X)  : origin=0x00010000 length=0x00000020
     * This updated memory app makes it so that the application starts at address
     * 0x00010000, which is where the bootloader will be loading the application.
     */
    VECTORS (X)  : origin=0x00000000 length=0x00000020
    KERNEL  (RX) : origin=0x00010020 length=0x00008000
    FLASH0  (RX) : origin=0x00018020 length=0x0013FFE0
    STACKS  (RW) : origin=0x08000000 length=0x00001500
    KRAM    (RW) : origin=0x08001500 length=0x00000800
    RAM     (RW) : origin=(0x08001500+0x00000800) length=(0x0002EB00-0x00000800)
}

SECTIONS
{
    .intvecs : {} > VECTORS
    /* FreeRTOS Kernel in protected region of Flash */
    privileged_functions  : {} > KERNEL
    .cinit                : {} > KERNEL
    .pinit                : {} > KERNEL
    /* Rest of code to user mode flash region */
    freertos_system_calls : {} > FLASH0
    .text                 : {} > FLASH0
    .const                : {} > FLASH0
    /* FreeRTOS Kernel data in protected region of RAM */
    privileged_data       : {} > KRAM
    .kernelHEAP           : {} > RAM
    .bss                  : {} > RAM
    .data                 : {} > RAM
    .sysmem               : {} > RAM
}
