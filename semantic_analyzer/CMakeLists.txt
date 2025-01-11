# 创建 semantic_analyzer 的静态库
add_library(semantic_analyzer STATIC
    s_analyzer.c
    symbol_table.c
)

target_include_directories(semantic_analyzer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")