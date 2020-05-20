cmake_minimum_required(VERSION 3.10)
project(warning_processor)

set(CMAKE_CXX_STANDARD 17)

add_executable(warning_processor main.cpp warnings.h warnings.cpp)
target_link_libraries(warning_processor stdc++fs)
