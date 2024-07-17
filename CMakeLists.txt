cmake_minimum_required(VERSION 3.20)

project(TPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE src src/*.cpp include/*.hpp)
add_executable(t++ ${src})
target_include_directories(t++ PUBLIC include)
