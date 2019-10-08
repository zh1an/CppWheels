cmake_minimum_required(VERSION 3.0)
project(cpp_min_heap_timer)

set(CMAKE_CXX_STANDARD 11)

IF(WIN32)
    LINK_LIBRARIES(ws2_32)
ENDIF()

set(USE_MIN_HEAP_TIMER 1)
set(USE_SORTED_TIMER 0)

configure_file(
        "config.h.in"
        "${PROJECT_SOURCE_DIR}/source/config.h"
)

add_executable(
        cpp_min_heap_timer
        main.cpp
        source/data.hpp
        source/timer.cc
        source/timer.hpp
)