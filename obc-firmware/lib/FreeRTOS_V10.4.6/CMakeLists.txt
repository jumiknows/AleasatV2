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

target_link_libraries(FreeRTOS PUBLIC
    platform
)
