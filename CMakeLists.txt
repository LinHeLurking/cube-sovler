cmake_minimum_required(VERSION 3.16)
project(cube_solver)

set(CMAKE_CXX_STANDARD 17)

IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)
ENDIF()

add_subdirectory(extern/gtest)
include_directories(${gtest_SOURCE_DIR}/include)

add_executable(cube_solver cube_def.h utility.h main.cpp two_phase_solver.h)

add_executable(cube_solver_test
        extern/gtest/googletest/src/gtest_main.cc cube_def.h utility.h two_phase_solver.h test.cpp)
add_dependencies(cube_solver_test gtest)
target_link_libraries(cube_solver_test gtest)
