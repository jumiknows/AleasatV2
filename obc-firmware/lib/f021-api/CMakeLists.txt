add_library(f021-api
    source/Fapi_UserDefinedFunctions.c
)

target_include_directories(f021-api PUBLIC
    include
)

target_link_libraries(f021-api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/F021_API_CortexR4_BE.lib
)
