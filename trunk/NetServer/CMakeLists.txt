cmake_minimum_required(VERSION 2.6)

#服务器项目
PROJECT(NetServer)

set (SCR_LIST 
    NetServer.cpp
    ProactorNetHandler.cpp
    ProactorNetAcceptor.cpp
    ProactorNetConnector.cpp
    Session.cpp
    SessionManager.cpp
    )

IF(WIN32)
ELSEIF(UNIX)
    set (CMAKE_CXX_COMPILER gcc)
    set (LINK_LIB 
        /usr/lib/libACE.so
        )
    set (INC_DIR
        ../Common/
        )
ENDIF(WIN32)

include_directories(${INC_DIR})
add_library(NetServer SHARED ${SCR_LIST} ${SCRIPT_LIST})

IF(WIN32)
ELSEIF(UNIX)
    set_target_properties(NetServer PROPERTIES PREFIX "" SUFFIX ".so")
ENDIF(WIN32)

target_link_libraries(NetServer ${LINK_LIB})

