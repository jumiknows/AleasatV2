################################################################################
# SOURCES
################################################################################

set(. ${CMAKE_CURRENT_LIST_DIR}) # Location of this .cmake file

list(APPEND SRC_DIRS

    ${.}/hardwaredefs
    ${.}/featuredefs

    ${.}/device-drivers
    ${.}/device-drivers/ADIS16260_gyro
    ${.}/device-drivers/comms
    ${.}/device-drivers/eps
    ${.}/device-drivers/flash
    ${.}/device-drivers/gpio
    ${.}/device-drivers/gps
    ${.}/device-drivers/imu
    ${.}/device-drivers/magnetorquer

    ${.}/rtos

    ${.}/system
    ${.}/system/command
    ${.}/system/comms
    ${.}/system/filesystem
    ${.}/system/logging

    ${.}/tests
    ${.}/tms570

    ${.}/utils

    ${.}/orcasat
    ${.}/orcasat/hardware-drivers
    ${.}/orcasat/hardware-drivers/gps
    ${.}/orcasat/hardware-drivers/heartbeat
    ${.}/orcasat/hardware-drivers/mram
    ${.}/orcasat/hardware-drivers/rtc
    ${.}/orcasat/hardware-drivers/temperature
    ${.}/orcasat/hardware-mock
    ${.}/orcasat/interfaces
    ${.}/orcasat/rtos
    ${.}/orcasat/system
    ${.}/orcasat/system/nvct
    ${.}/orcasat/system/scheduler
    ${.}/orcasat/system/settings
    ${.}/orcasat/system/state
    ${.}/orcasat/system/telemetry
    ${.}/orcasat/system/watchdog
    ${.}/orcasat/tasks
    ${.}/orcasat/utilities
    ${.}/orcasat/printf
)

# Glob out all the source files
# NOTE: This is BAD practice, but I can't be bothered to write out every single source file path right now

foreach(SRC_DIR IN LISTS
    SRC_DIRS
)

    # C sources
    file(GLOB C_SRCS ${SRC_DIR}/*.c)
    list(APPEND SRC_FILES ${C_SRCS})
    # Assembly sources
    file(GLOB ASM_SRCS ${SRC_DIR}/*.asm)
    list(APPEND SRC_FILES ${ASM_SRCS})

endforeach()

# Generate Log IDs

add_custom_command(
    OUTPUT              generated/log_ids.h
    WORKING_DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND             pwd && python3 tools/logging/log_ids_h_gen.py
                            -i ${CMAKE_CURRENT_SOURCE_DIR}/tools/logging/log_ids.json
                            -t ${CMAKE_CURRENT_SOURCE_DIR}/tools/logging/log_ids_template.h
                            -o ${CMAKE_CURRENT_BINARY_DIR}/generated/log_ids.h
    DEPENDS             ./tools/logging/log_ids.json    ./tools/logging/log_ids_template.h
)
list(APPEND SRC_FILES
    generated/log_ids.h
)

# Main / ISRs

list(APPEND SRC_FILES
    ${.}/obc_main.c
    ${.}/isr.asm
)

################################################################################
# TARGET
################################################################################

target_sources(${PROJECT_NAME} PRIVATE
    ${SRC_FILES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    SYSTEM_TESTS
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SRC_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

# Link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    FreeRTOS
    littlefs
)

if (USE_F021API EQUAL 1)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        F021-API
    )
endif()
