cmake_minimum_required(VERSION 3.0)
project(cpp_min_heap_timer)

set(CMAKE_CXX_STANDARD 11)

IF(WIN32)
    LINK_LIBRARIES(ws2_32)
ENDIF()

add_executable(
        cpp_min_heap_timer
        main.cpp
        source/minHeapTimer.cc
        source/minHeapTimer.hpp
)