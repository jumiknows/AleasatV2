add_library(printf STATIC
    printf.c
)

target_include_directories(printf PUBLIC
    ./
)
