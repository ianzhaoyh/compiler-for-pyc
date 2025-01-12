# 创建 scanner 的静态库
add_library(scanner STATIC
    scanner.c
)

# 如果 scanner 使用了 error_handler.h 中的函数，可以链接
target_include_directories(scanner PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")