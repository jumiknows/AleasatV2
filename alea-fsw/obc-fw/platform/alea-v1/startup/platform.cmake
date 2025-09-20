set(. ${CMAKE_CURRENT_LIST_DIR}) # Location of this .cmake file

target_sources(platform PRIVATE
    # Custom data abort handler is used in platform/common/exception/dabort/dabort.asm
    ${.}/halcogen/source/dabort.asm
    ${.}/halcogen/source/errata_SSWF021_45.c
    ${.}/halcogen/source/esm.c
    ${.}/halcogen/source/gio.c
    ${.}/halcogen/source/notification.c
    ${.}/halcogen/source/pinmux.c
    ${.}/halcogen/source/sys_core.asm
    ${.}/halcogen/source/sys_dma.c
    ${.}/halcogen/source/sys_mpu.asm
    ${.}/halcogen/source/sys_pcr.c
    ${.}/halcogen/source/sys_phantom.c
    ${.}/halcogen/source/sys_pmm.c
    ${.}/halcogen/source/sys_pmu.asm
    ${.}/halcogen/source/sys_selftest.c
    ${.}/halcogen/source/sys_startup.c
    ${.}/halcogen/source/sys_vim.c
    ${.}/halcogen/source/system.c

    # The following HALCoGen files are excluded from the build since we provide our own implementation
    #
    #     - sys_intvecs.asm  |  Exception vectors are contained in platform/common/ files
    #     - sys_main.c       |  Application layer provides the main function
)

target_include_directories(platform PUBLIC
    ${.}/halcogen/include
)
