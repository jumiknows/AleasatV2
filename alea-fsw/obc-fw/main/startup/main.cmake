set(. ${CMAKE_CURRENT_LIST_DIR}) # Location of this .cmake file

target_sources(${PROJECT_NAME} PRIVATE
    ${.}/obc_main.c
    ${.}/isr.asm
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${.}/
)
