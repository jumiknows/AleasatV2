add_library(littlefs
    lfs.c
    lfs_util.c
)

target_include_directories(littlefs PUBLIC
    ./
)
