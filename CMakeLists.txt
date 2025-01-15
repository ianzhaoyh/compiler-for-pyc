cmake_minimum_required(VERSION 3.10)
project(CompilerProject C)

#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_C_FLAGS_DEBUG "-g -Wall -O0")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
# 设置 C 标准
set(CMAKE_C_STANDARD 99)

# 包含子目录
add_subdirectory("parser")
add_subdirectory("scanner_pyc")
add_subdirectory("semantic_analyzer")
