add_library(littlefs STATIC
    lfs.c
    lfs_util.c
)

target_include_directories(littlefs PUBLIC
    ./
)

target_compile_definitions(littlefs PUBLIC
    LFS_NO_MALLOC
    LFS_NO_DEBUG
    LFS_NO_WARN
    LFS_NO_ERROR
    LFS_NO_ASSERT
)

if (USE_LITTLEFS_RDONLY EQUAL 1)
    target_compile_definitions(littlefs PUBLIC
        LFS_READONLY
    )
endif()
