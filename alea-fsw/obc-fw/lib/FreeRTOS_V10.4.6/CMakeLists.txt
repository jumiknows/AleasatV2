add_library(FreeRTOS STATIC
    source/croutine.c
    source/event_groups.c
    source/list.c
    source/mpu_wrappers.c
    source/queue.c
    source/stream_buffer.c
    source/tasks.c
    source/timers.c
    portable/port.c
    portable/portASM.asm
)

target_include_directories(FreeRTOS PUBLIC
    ./
    include
    portable
)

target_compile_options(FreeRTOS PRIVATE
    --diag_suppress=195  # zero used for undefined preprocessing
    --diag_suppress=2142 # comparison between signed and unsigned operands
)

target_link_libraries(FreeRTOS PUBLIC
    platform
)
