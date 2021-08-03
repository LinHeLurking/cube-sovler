# cmake_minimum_required(VERSION <specify CMake version here>)
project(cube_solver)

set(CMAKE_CXX_STANDARD 17)

add_executable(cube_solver cube_def.h utility.h test.cpp)