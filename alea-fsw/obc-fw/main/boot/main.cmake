set(. ${CMAKE_CURRENT_LIST_DIR}) # Location of this .cmake file

target_sources(${PROJECT_NAME} PRIVATE
    ${.}/obc_main.c
    ${.}/isr.asm
    ${.}/boot_mt25ql.c
    ${.}/boot_mibspi.c
    ${.}/boot_lfs.c
    ${.}/boot_updater.c
    ${.}/boot_fapi.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${.}/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    F021-API
    littlefs
)
