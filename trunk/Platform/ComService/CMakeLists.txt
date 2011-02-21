cmake_minimum_required(VERSION 2.6)

#INCLUDE (../Platform.cmake)

PROJECT(ComService)

IF(WIN32)
ELSEIF(UNIX)
    set (INC_DIR
            ../Include)
    set (SRC_LIST
            Buffer.cpp
            Randomize.cpp
            Encrypt.cpp
#   ComService.cpp
    )
ENDIF(WIN32)



include_directories(${INC_DIR})
add_library(ComService SHARED ${SRC_LIST})

IF(WIN32)
ELSEIF(UNIX)
    set_target_properties(ComService PROPERTIES PREFIX "" SUFFIX ".so")
ENDIF(WIN32)

target_link_libraries(ComService ${LINK_LIB})
