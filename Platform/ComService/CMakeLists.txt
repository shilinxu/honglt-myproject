cmake_minimum_required(VERSION 2.6)

#INCLUDE (../Platform.cmake)

PROJECT(ComService)

IF(WIN32)
    set (INC_DIR
            ../Include)
    set (SRC_LIST
            Buffer.cpp
            Randomize.cpp
            Encrypt.cpp
            ComService.cpp)
    set (HDR_LIST
            ../Include/Buffer.h
            ../Include/Randomize.h
            ../Include/Encrypt.h
            ../Include/ComService.h)
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

#set(CMAKE_BUILD_TYPE "Debug")

#IF(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DCOM_SERVICE_Export)
    set (CMAKE_CXX_FLAGS_DEBUG "/MTd")
#ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
#    set (CMAKE_CXX_FLAGS_DEBUG "MT")
#ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)


include_directories(${INC_DIR})
add_library(ComService SHARED ${SRC_LIST} ${HDR_LIST})

IF(WIN32)
ELSEIF(UNIX)
    set_target_properties(ComService PROPERTIES PREFIX "" SUFFIX ".so")
ENDIF(WIN32)

target_link_libraries(ComService ${LINK_LIB})
