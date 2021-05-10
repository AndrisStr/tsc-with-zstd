add_library(zstdwrap zstd_zlibwrapper.c)
target_link_libraries(zstdwrap zstd)