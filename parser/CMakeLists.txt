# 创建 parser 可执行文件
add_executable(parser
    parse.c
    syntax_tree.c
)

# 连接 scanner 和 semantic_analyzer 的静态库
target_link_libraries(parser scanner)
#target_link_libraries(parser scanner semantic_analyzer)