cmake_minimum_required(VERSION 3.20)

project(TPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core irreader)

file(GLOB_RECURSE src src/*.cpp include/*.hpp)
add_executable(t++ ${src})
target_include_directories(t++ PUBLIC include)
target_link_libraries(t++ PUBLIC ${llvm_libs})
