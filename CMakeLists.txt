cmake_minimum_required(VERSION 3.28)
project(break_control)

set(CMAKE_CXX_STANDARD 23)

add_executable(break_control main.cpp)
target_link_libraries(break_control winmm.lib)
