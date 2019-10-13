cmake_minimum_required(VERSION 3.15)
project(libevent_client)

set(CMAKE_CXX_STANDARD 11)

IF(WIN32)
    link_libraries(ws2_32)
ENDIF()



add_executable(libevent_client main.cpp)