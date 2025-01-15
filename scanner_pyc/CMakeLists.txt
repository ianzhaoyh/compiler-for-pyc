add_library(scanner STATIC
    scanner.c
) #把 scanner.c 编译成名为 scanner 的静态库（.a 文件）

target_include_directories(scanner PUBLIC
    ${PROJECT_SOURCE_DIR}/scanner_pyc
)#设置 头文件路径，方便其他模块（如 parser 或 semantic_analyzer）引用 scanner_pyc 目录下的头文件。PUBLIC 表示这个路径对所有链接到这个库的目标都可见。