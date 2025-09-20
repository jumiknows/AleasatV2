cmake_minimum_required(VERSION 3.10)

if (VERBOSE EQUAL 1)
    set(CMAKE_VERBOSE_MAKEFILE on)
endif()

message(STATUS "Toolchain:  ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Platform:   ${PLATFORM}")
message(STATUS "Target:     ${TARGET}")
message(STATUS "Flash Slot: ${FLASH_SLOT_NAME}")
message(STATUS "Githash:    ${GITHASH}")
message(STATUS "Version:    ${VERSION}")

if (GIT_DIRTY EQUAL 1)
    message(STATUS "Repo State: Dirty")
else()
    message(STATUS "Repo State: Clean")
endif()

if (STANDALONE EQUAL 1)
    message(STATUS "Standalone: Yes")
else()
    message(STATUS "Standalone: No")
endif()

if (COMMS_OVER_SERIAL EQUAL 1)
    message(STATUS "COMMS Mode: OBC Serial")
else()
    message(STATUS "COMMS Mode: SPI")
endif()

################################################################################
# PROJECT
################################################################################

# Project Name and Version
project(${PROJECT_NAME} LANGUAGES C ASM VERSION ${VERSION})

################################################################################
# ARGUMENTS
################################################################################

set(USE_RTOS 0)
set(USE_F021API 0)
set(USE_LITTLEFS 0)
set(USE_LITTLEFS_RDONLY 0)
set(USE_DSPLIB 0)
set(USE_MINMEA 0)

# The configuration of the stack pointers (for each processor mode)
# which occurs in _coreInitStackPointer_ in sys_core.asm MUST be
# compatible with the origin and length configured here.

set(HEAP_SIZE 0x800)
set(STACK_SIZE 0x800)

if ("${PLATFORM}" STREQUAL "launchpad")
    set(FW_PLATFORM "FW_PLATFORM_LAUNCHPAD")
    set(PLATFORM_DEFINE PLATFORM_LAUNCHPAD_1224)
elseif ("${PLATFORM}" STREQUAL "alea-v1")
    set(FW_PLATFORM "FW_PLATFORM_ALEAV1")
    set(PLATFORM_DEFINE PLATFORM_ALEA_V1)
else()
    message(FATAL_ERROR "invalid PLATFORM: ${PLATFORM}")
endif()

if ("${TARGET}" STREQUAL "startup")

    set(MAIN_DIR "main/startup")
    set(FW_TARGET "FW_TARGET_STARTUP")

    if ("${FLASH_SLOT_NAME}" STREQUAL "A")
        set(FLASH_SLOT 0)
    else()
        message(FATAL_ERROR "invalid FLASH_SLOT_NAME: ${TARGET}")
    endif()

elseif ("${TARGET}" STREQUAL "boot")

    set(MAIN_DIR "main/boot")
    set(FW_TARGET "FW_TARGET_BOOT")

    set(USE_F021API 1)
    set(USE_LITTLEFS 1)
    set(USE_LITTLEFS_RDONLY 1)

    # The boot fw requries a larger stack size
    # This is also reflected in the HALCoGen platform files
    set(STACK_SIZE 0xD00)

    if ("${FLASH_SLOT_NAME}" STREQUAL "A")
        set(FLASH_SLOT 1)
    elseif ("${FLASH_SLOT_NAME}" STREQUAL "B")
        set(FLASH_SLOT 2)
    else()
        message(FATAL_ERROR "invalid FLASH_SLOT_NAME: ${TARGET}")
    endif()

elseif ("${TARGET}" STREQUAL "core")

    set(MAIN_DIR "main/app")
    set(FW_TARGET "FW_TARGET_CORE")

    set(USE_RTOS 1)
    set(USE_LITTLEFS 1)
    set(USE_DSPLIB 1)
    set(USE_MINMEA 1)

    if ("${FLASH_SLOT_NAME}" STREQUAL "A")
        set(FLASH_SLOT 3)
    else()
        message(FATAL_ERROR "invalid FLASH_SLOT_NAME: ${TARGET}")
    endif()

elseif ("${TARGET}" STREQUAL "ext")

    set(MAIN_DIR "main/app")
    set(FW_TARGET "FW_TARGET_EXT")

    set(USE_RTOS 1)
    set(USE_LITTLEFS 1)
    set(USE_DSPLIB 1)
    set(USE_MINMEA 1)

    if ("${FLASH_SLOT_NAME}" STREQUAL "A")
        set(FLASH_SLOT 4)
    elseif ("${FLASH_SLOT_NAME}" STREQUAL "B")
        set(FLASH_SLOT 5)
    else()
        message(FATAL_ERROR "invalid FLASH_SLOT_NAME: ${TARGET}")
    endif()

else()
    message(FATAL_ERROR "invalid TARGET: ${TARGET}")
endif()

################################################################################
# SUBDIRECTORIES
################################################################################

add_subdirectory(platform)

if (USE_RTOS EQUAL 1)
    add_subdirectory(lib/FreeRTOS_V10.4.6)
endif()

if (USE_F021API EQUAL 1)
    add_subdirectory(lib/f021-api)
endif()

if (USE_LITTLEFS EQUAL 1)
    add_subdirectory(lib/littlefs-2.4.2)
endif()

if (USE_DSPLIB EQUAL 1)
    add_subdirectory(lib/dsplib)
endif()

if (USE_MINMEA EQUAL 1)
    add_subdirectory(lib/minmea)
endif()


################################################################################
# SOURCES / TARGETS
################################################################################

configure_file(common/firmware/fw_defines.h.in generated/fw_defines.h)

# Executable
add_executable(${PROJECT_NAME}
    main/main.c
    common/firmware/fw_structs.c
    common/util/asm_utils.asm
    common/util/obc_crc.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    main
    common
    common/firmware
    common/util
    common/flashdefs
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _DEBUG
    CFG_FLASH_SLOT=${FLASH_SLOT}
    COMMS_OVER_SERIAL=${COMMS_OVER_SERIAL}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    --diag_suppress=880 # parameter was never referenced
    --diag_suppress=195 # zero used for undefined preprocessing
)

include(${MAIN_DIR}/main.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
    platform
    ${CMAKE_CURRENT_SOURCE_DIR}/main/linker.cmd
)

target_link_directories(${PROJECT_NAME} PRIVATE
    common/firmware
)

target_link_options(${PROJECT_NAME} PRIVATE
    --heap_size=${HEAP_SIZE}
    --stack_size=${STACK_SIZE}

    # The SHELL quoting is required to prevent option de-duplication
    # (see https://cmake.org/cmake/help/latest/command/target_link_options.html#option-de-duplication)
    "SHELL:--define STACK_SIZE=${STACK_SIZE}"
    "SHELL:--define CFG_USE_RTOS=${USE_RTOS}"
    "SHELL:--define CFG_FLASH_SLOT=${FLASH_SLOT}"
    "SHELL:--define CFG_STANDALONE=${STANDALONE}"
    "SHELL:--define CFG_USE_F021API=${USE_F021API}"
)

################################################################################
# POST BUILD
################################################################################

# 1) Convert output .elf file to a Motorola s-record with 32-bit addresses (.mot)
# 2) Generate the firmware header and overwrite the .mot file to include the header
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_HEX} --motorola=3 --memwidth=32 --romwidth=32 --map=${PROJECT_NAME}.mot.map --outfile=${PROJECT_NAME}.mot $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/python/alea-obcfw/scripts/obcfw_post_build.py -i ${PROJECT_NAME}.mot -o ${PROJECT_NAME}.mot -e ${PROJECT_NAME}.elf
)
