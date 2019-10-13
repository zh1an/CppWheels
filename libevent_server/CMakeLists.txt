cmake_minimum_required(VERSION 3.15)
project(libevent_server)

set(CMAKE_CXX_STANDARD 11)


include_directories("/usr/local/include")

link_directories("/usr/local/lib")
link_libraries(event event_core event_extra glog)

add_executable(libevent_server main.cpp)