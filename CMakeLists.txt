cmake_minimum_required(VERSION 3.10)
project(CompilerProject C)

# 设置 C 标准
set(CMAKE_C_STANDARD 99)

# 包含子目录
add_subdirectory("parser")
add_subdirectory("scanner_pyc")
#add_subdirectory("semantic_analyzer")
