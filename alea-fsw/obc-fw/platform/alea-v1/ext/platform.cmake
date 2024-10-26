set(. ${CMAKE_CURRENT_LIST_DIR}) # Location of this .cmake file

target_sources(platform PRIVATE
    ${.}/halcogen/source/adc.c
    ${.}/halcogen/source/can.c
    ${.}/halcogen/source/dabort.asm
    ${.}/halcogen/source/errata_SSWF021_45.c
    ${.}/halcogen/source/esm.c
    ${.}/halcogen/source/gio.c
    ${.}/halcogen/source/het.c
    ${.}/halcogen/source/i2c.c
    ${.}/halcogen/source/mibspi.c
    ${.}/halcogen/source/notification.c
    ${.}/halcogen/source/pinmux.c
    ${.}/halcogen/source/rti.c
    ${.}/halcogen/source/sci.c
    ${.}/halcogen/source/spi.c
    ${.}/halcogen/source/sys_core.asm
    ${.}/halcogen/source/sys_dma.c
    ${.}/halcogen/source/sys_mpu.asm
    ${.}/halcogen/source/sys_pcr.c
    ${.}/halcogen/source/sys_phantom.c
    ${.}/halcogen/source/sys_pmm.c
    ${.}/halcogen/source/sys_pmu.asm
    ${.}/halcogen/source/sys_selftest.c
    ${.}/halcogen/source/sys_startup.c
    ${.}/halcogen/source/sys_vim.c
    ${.}/halcogen/source/system.c

    # The following HALCoGen files are excluded from the build since we provide our own implementation
    #
    #     - sys_intvecs.asm  |  Exception vectors are contained in platform/common/ files
    #     - sys_main.c       |  Application layer provides the main function
)

target_include_directories(platform PUBLIC
    ${.}/halcogen/include
)

# HALCoGen generates a vPortPreemptiveTick function in the rti.c library (and assigns the RTI interrupt vector to this function)
# Since the RTOS also defines this function we need to rename the HALCoGen one (only in rti.c not in sys_vim.c)
# This would be easier if the linker supported multiple definitions but unfortunately the TI linker always generates a fatal error for multiple definitions
set_source_files_properties(${.}/halcogen/source/rti.c PROPERTIES COMPILE_DEFINITIONS vPortPreemptiveTick=HALCOGEN_vPortPreemptiveTick)
