set(PLATFORM_DIRECTORY "${PLATFORM}/${TARGET}")

add_library(platform STATIC
    common/exception/exc_ram.asm
    common/exception/dabort/dabort.asm
    common/exception/dabort/dabort.c

    common/startup/entrypoint.asm
    common/startup/startup_crt.c
)

if (STANDALONE EQUAL 1)
    target_sources(platform PRIVATE
        common/exception/exc_flash.asm
    )
endif()

target_include_directories(platform PUBLIC
    common
    common/exception
    common/exception/dabort
    common/startup
)

target_compile_definitions(platform PUBLIC
    ${PLATFORM_DEFINE}
)

target_compile_options(platform PRIVATE
    --diag_suppress=195 # zero used for undefined preprocessing
    --diag_suppress=880 # parameter was never referenced
)

include(${PLATFORM_DIRECTORY}/platform.cmake)
