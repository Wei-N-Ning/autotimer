cmake_minimum_required(VERSION 3.10)
project(autotimer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(src)
add_subdirectory(examples)