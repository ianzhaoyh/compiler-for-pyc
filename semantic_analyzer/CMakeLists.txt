# 设置 semantic_analyzer 可执行文件
add_executable(semantic_analyzer
    analyzer.c
    main.c
    s_analyzer.c
    symbol_table.c
) # 编译 main.c、analyzer.c、s_analyzer.c、symbol_table.c 生成可执行文件 semantic_analyzer

# 包含头文件
target_include_directories(semantic_analyzer PUBLIC
    ${PROJECT_SOURCE_DIR}/parser
    ${PROJECT_SOURCE_DIR}/scanner_pyc
    ${PROJECT_SOURCE_DIR}/semantic_analyzer
)

# 链接 parser 和 scanner 库
target_link_libraries(semantic_analyzer parser scanner)