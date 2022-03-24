if (PLATFORM STREQUAL "launchpad")
    set(PLATFORM_DEFINE    PLATFORM_LAUNCHPAD_1224)
    set(PLATFORM_DIRECTORY launchpad)
elseif (PLATFORM STREQUAL "alea-v1")
    set(PLATFORM_DEFINE    PLATFORM_ALEA_V1)
    set(PLATFORM_DIRECTORY alea-v1)
else()
    message(FATAL_ERROR "invalid PLATFORM: ${PLATFORM}")
endif()

message(STATUS "Building for platform ${PLATFORM}")

add_library(platform
    ${PLATFORM_DIRECTORY}/halcogen/source/adc.c
    ${PLATFORM_DIRECTORY}/halcogen/source/can.c
    ${PLATFORM_DIRECTORY}/halcogen/source/dabort.asm
    ${PLATFORM_DIRECTORY}/halcogen/source/errata_SSWF021_45.c
    ${PLATFORM_DIRECTORY}/halcogen/source/esm.c
    ${PLATFORM_DIRECTORY}/halcogen/source/gio.c
    ${PLATFORM_DIRECTORY}/halcogen/source/het.c
    ${PLATFORM_DIRECTORY}/halcogen/source/i2c.c
    ${PLATFORM_DIRECTORY}/halcogen/source/mibspi.c
    ${PLATFORM_DIRECTORY}/halcogen/source/notification.c
    ${PLATFORM_DIRECTORY}/halcogen/source/pinmux.c
    ${PLATFORM_DIRECTORY}/halcogen/source/rti.c
    ${PLATFORM_DIRECTORY}/halcogen/source/sci.c
    ${PLATFORM_DIRECTORY}/halcogen/source/spi.c
    ${PLATFORM_DIRECTORY}/halcogen/source/sys_core.asm
    ${PLATFORM_DIRECTORY}/halcogen/source/sys_dma.c
    ${PLATFORM_DIRECTORY}/halcogen/source/sys_intvecs.asm
    # Application layer provides the main function - don't use the HALCoGen one
    # ${PLATFORM_DIRECTORY}/halcogen/source/sys_main.c
    ${PLATFORM_DIRECTORY}/halcogen/source/sys_mpu.asm
    ${PLATFORM_DIRECTORY}/halcogen/source/sys_pcr.c
    ${PLATFORM_DIRECTORY}/halcogen/source/sys_phantom.c
    ${PLATFORM_DIRECTORY}/halcogen/source/sys_pmm.c
    ${PLATFORM_DIRECTORY}/halcogen/source/sys_pmu.asm
    ${PLATFORM_DIRECTORY}/halcogen/source/sys_selftest.c
    ${PLATFORM_DIRECTORY}/halcogen/source/sys_startup.c
    ${PLATFORM_DIRECTORY}/halcogen/source/sys_vim.c
    ${PLATFORM_DIRECTORY}/halcogen/source/system.c
)

target_include_directories(platform PUBLIC
    ${PLATFORM_DIRECTORY}/halcogen/include
)

target_compile_definitions(platform PUBLIC
    ${PLATFORM_DEFINE}
)

target_link_libraries(platform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIRECTORY}/linker.cmd
)

# HALCoGen generates a vPortPreemptiveTick function in the rti.c library (and assigns the RTI interrupt vector to this function)
# Since the RTOS also defines this function we need to rename the HALCoGen one (only in rti.c not in sys_vim.c)
# This would be easier if the linker supported multiple definitions but unfortunately the TI linker always generates a fatal error for multiple definitions
set_source_files_properties(${PLATFORM_DIRECTORY}/halcogen/source/rti.c PROPERTIES COMPILE_DEFINITIONS vPortPreemptiveTick=HALCOGEN_vPortPreemptiveTick)
