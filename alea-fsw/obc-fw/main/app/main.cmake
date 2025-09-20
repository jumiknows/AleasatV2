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
    ${.}/device-drivers/mram
    ${.}/device-drivers/gpio
    ${.}/device-drivers/gps
    ${.}/device-drivers/imu
    ${.}/device-drivers/magnetorquer
    ${.}/device-drivers/rtc

    ${.}/device-mock

    ${.}/rtos

    ${.}/system
    ${.}/system/adcs
    ${.}/system/adcs/sun_model
    ${.}/system/adcs/wmm
    ${.}/system/adcs/adcs_math
    ${.}/system/adcs/attitude_determination
    ${.}/system/cmd_sys
    ${.}/system/cmd_sys/impl
    ${.}/system/comms
    ${.}/system/filesystem
    ${.}/system/gndstn
    ${.}/system/health
    ${.}/system/logging
    ${.}/system/rtc_scheduler
    ${.}/system/serial
    ${.}/system/telem
    ${.}/system/telem/impl

    ${.}/tests
    ${.}/tms570

    ${.}/utils

    ${.}/orcasat
    ${.}/orcasat/hardware-drivers/heartbeat
    ${.}/orcasat/hardware-mock
    ${.}/orcasat/system
    ${.}/orcasat/system/nvct
    ${.}/orcasat/system/settings
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
    OUTPUT              generated/logger.h
    WORKING_DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND             ./python/alea-obcfw/scripts/obcfw_codegen.py
                            log
                            -o ${CMAKE_CURRENT_BINARY_DIR}/generated
    DEPENDS             ./python/alea-obcfw/alea/obcfw/log/data/log_specs.json
)
list(APPEND SRC_FILES
    generated/logger.h
)

# Generate Command System Files

add_custom_command(
    OUTPUT              generated/cmd_sys_gen.h
                        generated/cmd_sys_gen.c
    WORKING_DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND             ./python/alea-obcfw/scripts/obcfw_codegen.py
                            cmd_sys
                            -o ${CMAKE_CURRENT_BINARY_DIR}/generated
    DEPENDS             ./python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys.json
                        ./python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys_test.json
)
list(APPEND SRC_FILES
    generated/cmd_sys_gen.h
    generated/cmd_sys_gen.c
)

# Generate Task Files

add_custom_command(
    OUTPUT              generated/obc_tasks_ids_gen.h
                        generated/obc_tasks_gen.h
                        generated/obc_tasks_gen.c
    WORKING_DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND             ./python/alea-obcfw/scripts/obcfw_codegen.py
                            task
                            -o ${CMAKE_CURRENT_BINARY_DIR}/generated
    DEPENDS             ./python/alea-obcfw/alea/obcfw/task/data/obc_tasks.json
)
list(APPEND SRC_FILES
    generated/obc_tasks_ids_gen.h
    generated/obc_tasks_gen.h
    generated/obc_tasks_gen.c
)

# Generate Telem Files

add_custom_command(
    OUTPUT              generated/telem_gen.h
                        generated/telem_gen.c
    WORKING_DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND             ./python/alea-obcfw/scripts/obcfw_codegen.py
                            telem
                            -o ${CMAKE_CURRENT_BINARY_DIR}/generated
    DEPENDS             ./python/alea-obcfw/alea/obcfw/telem/data/telem.json
)
list(APPEND SRC_FILES
    generated/telem_gen.h
    generated/telem_gen.c
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
    minmea
)

if (USE_F021API EQUAL 1)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        F021-API
    )
endif()
