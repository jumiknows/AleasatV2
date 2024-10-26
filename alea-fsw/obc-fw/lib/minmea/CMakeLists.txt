add_library(minmea STATIC
    minmea.c
)

target_include_directories(minmea PUBLIC
    ./
)

target_compile_definitions(minmea PUBLIC
    MINMEA_INCLUDE_COMPAT=1
)