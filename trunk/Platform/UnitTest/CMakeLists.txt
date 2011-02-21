cmake_minimum_required(VERSION 2.6)

#INCLUDE (../Platform.cmake)

#测试项目
PROJECT(UnitTest)

##执行文件输出目录
#set (EXECUTABLE_OUTPUT_PATH ../../Bin/)
#
##中间文件存放目录
#set (PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/OBJ)

IF(WIN32)
ELSEIF(UNIX)
    set (INC_DIR
            ../Include)
    #    set (LINK_LIB 
    #        #${LIBRARY_OUTPUT_PATH}/ComService.so
    #        /home/honglt/MyProject/Platform/Lib/ComService.so
    #        )
ENDIF(WIN32)

#set (CMAKE_CXX_FLAGS_DEBUG "-ggdb")
#set (CMAKE_BUILD_TYPE "Debug")

#aux_source_directory(UnitTest TEST_SRC)
set (TEST_SRC
    TestCase.cpp
    ${TEST_DEST}
    )

include_directories(${INC_DIR})
add_executable(UnitTest ${SCR_LIST} ${TEST_SRC})

target_link_libraries(UnitTest ${LINK_LIB})
