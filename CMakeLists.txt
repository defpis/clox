cmake_minimum_required(VERSION 3.27)

project(clox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(3rd)

add_executable(lox_run lox_run.cpp)
target_link_libraries(lox_run lox)
target_include_directories(lox_run PUBLIC src)
