cmake_minimum_required(VERSION 2.6)

PROJECT(Platform)

set (EXECUTABLE_OUTPUT_PATH ../Bin)
set (LIBRARY_OUTPUT_PATH ../Lib)

set (PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/OBJ)

IF(WIN32)
ELSEIF(UNIX)
    set (Platform_SOURCE_DIR /home/honglt/MyProject/Platform)
ENDIF(WIN32)

set (CMAKE_CXX_COMPLIER gcc)

set (INC_DIR ${Platform_SOURCE_DIR}/Include)
set (CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set (CMAKE_BUILD_TYPE "Debug")

include_directories(${INC_DIR})
