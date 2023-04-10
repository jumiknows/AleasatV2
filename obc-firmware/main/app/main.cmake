################################################################################
# SOURCES
################################################################################

set(. ${CMAKE_CURRENT_LIST_DIR}) # Location of this .cmake file

list(APPEND SRC_DIRS

    ${.}/hardwaredefs
    ${.}/featuredefs

    ${.}/device-drivers
    ${.}/device-drivers/ADIS16260_gyro
    ${.}/device-drivers/camera
    ${.}/device-drivers/comms
    ${.}/device-drivers/eps
    ${.}/device-drivers/flash
    ${.}/device-drivers/gpio
    ${.}/device-drivers/gps
    ${.}/device-drivers/imu
    ${.}/device-drivers/magnetorquer

    ${.}/device-mock

    ${.}/rtos

    ${.}/system
    ${.}/system/cmd_sys
    ${.}/system/cmd_sys/impl
    ${.}/system/comms
    ${.}/system/filesystem
    ${.}/system/logging
    ${.}/system/rtc_scheduler
    ${.}/system/serial

    ${.}/tests
    ${.}/tms570

    ${.}/utils

    ${.}/orcasat
    ${.}/orcasat/hardware-drivers
    ${.}/orcasat/hardware-drivers/gps
    ${.}/orcasat/hardware-drivers/heartbeat
    ${.}/orcasat/hardware-drivers/mram
    ${.}/orcasat/hardware-drivers/rtc
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

# Generate Logging Files

add_custom_command(
    OUTPUT              generated/log_gen.h
    WORKING_DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND             pwd && python3 tools/logging/log_gen.py
                            -i ${CMAKE_CURRENT_SOURCE_DIR}/tools/logging/log_specs.json
                            -t ${CMAKE_CURRENT_SOURCE_DIR}/tools/logging/log_gen_template.h
                            -o ${CMAKE_CURRENT_BINARY_DIR}/generated/log_gen.h
    DEPENDS             ./tools/logging/log_specs.json
                        ./tools/logging/log_gen_template.h
)
list(APPEND SRC_FILES
    generated/log_gen.h
)

# Generate Command System Files

add_custom_command(
    OUTPUT              generated/cmd_sys_gen.h
                        generated/cmd_sys_gen.c
    WORKING_DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND             python3 tools/cmd_sys/cmd_sys_gen.py
                            -i ${CMAKE_CURRENT_SOURCE_DIR}/tools/cmd_sys/cmd_sys.json
                               ${CMAKE_CURRENT_SOURCE_DIR}/tools/cmd_sys/cmd_sys_test.json
                            -o ${CMAKE_CURRENT_BINARY_DIR}/generated
    DEPENDS             ./tools/cmd_sys/cmd_sys.json
                        ./tools/cmd_sys/cmd_sys_test.json
                        ./tools/cmd_sys/cmd_sys_gen.py
                        ./tools/cmd_sys/templates/cmd_sys_gen_template.h
                        ./tools/cmd_sys/templates/cmd_sys_gen_template.c
                        ./tools/cmd_sys/templates/macros.c
)
list(APPEND SRC_FILES
    generated/cmd_sys_gen.h
    generated/cmd_sys_gen.c
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
    DSPLIB
)

if (USE_F021API EQUAL 1)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        F021-API
    )
endif()
