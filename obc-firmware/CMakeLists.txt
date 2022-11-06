cmake_minimum_required(VERSION 3.10)

# Uncomment to print out compile commands
# set(CMAKE_VERBOSE_MAKEFILE on)

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

################################################################################
# ARGUMENTS
################################################################################

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

    set(FW_TARGET "FW_TARGET_STARTUP")
    set(USE_RTOS 0)
    if ("${FLASH_SLOT_NAME}" STREQUAL "A")
        set(FLASH_SLOT 0)
    else()
        message(FATAL_ERROR "invalid FLASH_SLOT_NAME: ${TARGET}")
    endif()

elseif ("${TARGET}" STREQUAL "boot")

    set(FW_TARGET "FW_TARGET_BOOT")
    set(USE_RTOS 0)
    if ("${FLASH_SLOT_NAME}" STREQUAL "A")
        set(FLASH_SLOT 1)
    elseif ("${FLASH_SLOT_NAME}" STREQUAL "B")
        set(FLASH_SLOT 2)
    else()
        message(FATAL_ERROR "invalid FLASH_SLOT_NAME: ${TARGET}")
    endif()

elseif ("${TARGET}" STREQUAL "core")

    set(FW_TARGET "FW_TARGET_CORE")
    set(USE_RTOS 1)
    if ("${FLASH_SLOT_NAME}" STREQUAL "A")
        set(FLASH_SLOT 3)
    else()
        message(FATAL_ERROR "invalid FLASH_SLOT_NAME: ${TARGET}")
    endif()

elseif ("${TARGET}" STREQUAL "ext")

    set(FW_TARGET "FW_TARGET_EXT")
    set(USE_RTOS 1)
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
# PROJECT
################################################################################

# Project Name and Version
project(${PROJECT_NAME} LANGUAGES C ASM VERSION ${VERSION})

################################################################################
# PROJECT SOURCES
################################################################################

# ./aleasat
list(APPEND SRC_DIRS_ALEASAT
    aleasat
    aleasat/device-drivers
    aleasat/device-drivers/ADIS16260_gyro
    aleasat/device-drivers/comms
    aleasat/device-drivers/eps
    aleasat/device-drivers/flash
    aleasat/device-drivers/gpio
    aleasat/device-drivers/gps
    aleasat/device-drivers/imu
    aleasat/device-drivers/magnetorquer
    aleasat/rtos
    aleasat/system
    aleasat/system/command
    aleasat/system/comms
    aleasat/system/filesystem
    aleasat/system/logging
    aleasat/tests
    aleasat/tms570

    common
    common/firmware
)

# ./orcasat
list(APPEND SRC_DIRS_ORCASAT
    orcasat
    orcasat/featuredefs
    orcasat/hardware-drivers
    orcasat/hardware-drivers/gps
    orcasat/hardware-drivers/heartbeat
    orcasat/hardware-drivers/mram
    orcasat/hardware-drivers/rtc
    orcasat/hardware-drivers/temperature
    orcasat/hardware-mock
    orcasat/hardwaredefs
    orcasat/interfaces
    orcasat/rtos
    orcasat/system
    orcasat/system/nvct
    orcasat/system/scheduler
    orcasat/system/settings
    orcasat/system/state
    orcasat/system/telemetry
    orcasat/system/watchdog
    orcasat/tasks
    orcasat/utilities
    # TODO: Fix printf
    lib/printf
)

add_subdirectory(platform)
add_subdirectory(lib/FreeRTOS_V10.4.6)
add_subdirectory(lib/f021-api)
add_subdirectory(lib/littlefs-2.4.2)
# add_subdirectory(lib/printf)
# add_subdirectory(lib/tiny-AES-c)

# Generated Files
configure_file(common/firmware/fw_defines.h.in generated/fw_defines.h)
add_custom_command(
    OUTPUT              generated/log_ids.h
    WORKING_DIRECTORY   ${CMAKE_CURRENT_LIST_DIR}
    COMMAND             python3 tools/logging/log_ids_h_gen.py
                            -i ${CMAKE_CURRENT_SOURCE_DIR}/tools/logging/log_ids.json
                            -t ${CMAKE_CURRENT_SOURCE_DIR}/tools/logging/log_ids_template.h
                            -o ${CMAKE_CURRENT_BINARY_DIR}/generated/log_ids.h
    DEPENDS             ./tools/logging/log_ids.json    ./tools/logging/log_ids_template.h
)

################################################################################
# GLOB FILES
################################################################################

# Glob out all the source files
# NOTE: This is BAD practice, but I can't be bothered to write out every single source file path right now

foreach(SRC_DIR IN LISTS
    SRC_DIRS_ALEASAT
    SRC_DIRS_ORCASAT
)

    # C sources
    file(GLOB C_SRCS ${SRC_DIR}/*.c)
    list(APPEND SRC_FILES ${C_SRCS})
    # Assembly sources
    file(GLOB ASM_SRCS ${SRC_DIR}/*.asm)
    list(APPEND SRC_FILES ${ASM_SRCS})

endforeach()

################################################################################
# TARGETS
################################################################################

# Executable
add_executable(${PROJECT_NAME}
    ${SRC_FILES}
    generated/fw_defines.h
    generated/log_ids.h
)

# Link Libraries
# target_link_libraries(${PROJECT_NAME} printf)
# target_link_libraries(${PROJECT_NAME} tiny-AES-c)
target_link_libraries(${PROJECT_NAME} littlefs)
target_link_libraries(${PROJECT_NAME} FreeRTOS)
target_link_libraries(${PROJECT_NAME} f021-api)
target_link_libraries(${PROJECT_NAME} platform)

################################################################################
# COMPILER OPTIONS
################################################################################

# Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${SRC_DIRS_ALEASAT}
    ${SRC_DIRS_ORCASAT}
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

# Defines
target_compile_definitions(${PROJECT_NAME} PRIVATE
    _DEBUG
    SYSTEM_TESTS
)

# Configure MISRA
target_compile_options(${PROJECT_NAME} PRIVATE
    # When changing check_misra, make sure to also update obc_misra.h
    --check_misra=all,-1.1,-2.2,-5.6,-5.7,-6.1,-10.1,-10.3,-10.6,-11.3,-11.4,-12.7,-12.13,-13.5,-14.7,-16.1,-16.7,-17.4,-18.4,-19.4,-19.7,-20.1,-16.3
    --misra_advisory=error
    --misra_required=error
)

################################################################################
# LINKER OPTIONS
################################################################################

target_link_options(${PROJECT_NAME} PRIVATE
    --heap_size=0x800
    --stack_size=0x800
)

################################################################################
# POST BUILD
################################################################################

# 1) Convert output .elf file to a Motorola s-record with 32-bit addresses (.mot)
# 2) Generate the firmware header and overwrite the .mot file to include the header
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_HEX} --motorola=3 --memwidth=32 --romwidth=32 --map=${PROJECT_NAME}.mot.map --outfile=${PROJECT_NAME}.mot $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/tools/build_sys/post_build.py -i ${PROJECT_NAME}.mot -o ${PROJECT_NAME}.mot
)
