#cmake_minimum_required(VERSION 2.6)

#执行文件输出目录
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../Bin/)
set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../Bin/)

#中间文件存放目录
set (PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/OBJ)

set (INC_DIR Include)
set (CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set (CMAKE_BUILD_TYPE "Debug")

include_directories(${INC_DIR})
