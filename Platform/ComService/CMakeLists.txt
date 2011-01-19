cmake_minimum_required(VERSION 2.6)

INCLUDE (../Platform.cmake)

PROJECT(ComService)

#IF(WIN32)
#ELSEIF(UNIX)
#ENDIF(WIN32)


set (SRC_LIST
    CString.cpp
    #ComService.cpp
    )

#include_directories(${INC_DIR})
add_library(ComService SHARED ${SRC_LIST})

IF(WIN32)
ELSEIF(UNIX)
    set_target_properties(ComService PROPERTIES PREFIX "" SUFFIX ".so")
ENDIF(WIN32)

target_link_libraries(ComService ${LINK_LIB})
