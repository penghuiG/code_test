cmake_minimum_required (VERSION 2.8)
 
project (my_tcp_server)

include_directories(./)


add_executable(command_pattern  ${CMAKE_CURRENT_SOURCE_DIR}/command_pattern.cpp)

