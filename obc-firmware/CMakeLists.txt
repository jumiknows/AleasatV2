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

if ("${PLATFORM}" STREQUAL "launchpad")
    set(FW_PLATFORM "FW_PLATFORM_LAUNCHPAD")
    set(PLATFORM_DEFINE PLATFORM_LAUNCHPAD_1224)
    set(USE_F021API 1)
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

################################################################################
# SOURCES / TARGETS
################################################################################

configure_file(common/firmware/fw_defines.h.in generated/fw_defines.h)

# Executable
add_executable(${PROJECT_NAME}
    main/main.c
    common/firmware/fw_structs.c
    common/util/asm_utils.asm
)

target_include_directories(${PROJECT_NAME} PRIVATE
    main
    common
    common/firmware
    common/util
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _DEBUG
    CFG_FLASH_SLOT=${FLASH_SLOT}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    # When changing check_misra, make sure to also update obc_misra.h
    --check_misra=all,-1.1,-2.2,-5.6,-5.7,-6.1,-8.1,-8.5,-8.7,-10.1,-10.3,-10.6,-11.3,-11.4,-12.2,-12.6,-12.7,-12.13,-13.5,-14.6,-14.7,-16.1,-16.7,-17.4,-18.4,-19.4,-19.7,-20.1,-16.3
    --misra_advisory=error
    --misra_required=error
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
    --heap_size=0x800
    --stack_size=0x800
    # The SHELL quoting is required to prevent option de-duplication
    # (see https://cmake.org/cmake/help/latest/command/target_link_options.html#option-de-duplication)
    "SHELL:--define CFG_USE_RTOS=${USE_RTOS}"
    "SHELL:--define CFG_FLASH_SLOT=${FLASH_SLOT}"
    "SHELL:--define CFG_STANDALONE=${STANDALONE}"
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
