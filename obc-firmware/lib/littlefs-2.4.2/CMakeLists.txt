add_library(littlefs STATIC
    lfs.c
    lfs_util.c
)

target_include_directories(littlefs PUBLIC
    ./
)

target_compile_definitions(littlefs PUBLIC
    # TODO
    # LFS_NO_MALLOC
)
