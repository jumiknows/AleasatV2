################################################################################
# SOURCES
################################################################################

list(APPEND SRC_DIRS
    source/BasicMathFunctions
    # source/CommonTables
    # source/ComplexMathFunctions
    # source/ControllerFunctions
    source/FastMathFunctions
    # source/FilteringFunctions
    source/MatrixFunctions
    # source/StatisticsFunctions
    # source/SupportFunctions
    # source/TransformFunctions
)

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

################################################################################
# TARGET
################################################################################

add_library(DSPLIB STATIC
    ${SRC_FILES}
    source/math_helper.c
)

target_compile_definitions(DSPLIB PUBLIC
    CCS
    FPU_PRESENT
    ARM_MATH_BIG_ENDIAN
    ARM_MATH_MATRIX_CHECK
    ARM_MATH_ROUNDING
)

target_include_directories(DSPLIB PUBLIC
    include
)

target_compile_options(DSPLIB PRIVATE
    --diag_suppress=2142 # comparison between signed and unsigned operands
)
