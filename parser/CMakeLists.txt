add_library(parser STATIC
    parse.c
    syntax_tree.c
    parse_print.c
) #把 parse.c、syntax_tree.c 和 parse_print.c 编译成名为 parser 的静态库（.a 文件）

target_include_directories(parser PUBLIC
    ${PROJECT_SOURCE_DIR}/parser
)#设置 头文件路径，方便其他模块（如 semantic_analyzer）引用 parser 目录下的头文件。PUBLIC 表示这个路径对所有链接到这个库的目标都可见。