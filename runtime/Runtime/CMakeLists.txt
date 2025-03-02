cmake_minimum_required(VERSION 3.29)
project(Runtime)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(src)
add_executable(Runtime main.cpp)
target_link_libraries(Runtime PRIVATE src)