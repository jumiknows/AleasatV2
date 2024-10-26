add_library(F021-API STATIC
    source/Fapi_UserDefinedFunctions.c
)

target_include_directories(F021-API PUBLIC
    include
)

target_link_libraries(F021-API PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/F021_API_CortexR4_BE.lib
)
