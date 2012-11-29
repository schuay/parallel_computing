cmake_minimum_required(VERSION 2.8.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../CMakeModules/")
enable_testing()

project(pc1)

find_package(Check REQUIRED)
find_package(OpenMP REQUIRED)

if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions("-Wall -Wextra -pedantic -std=c99")
endif()

include_directories(
    ${CHECK_INCLUDE_DIRS}
)

add_executable(recursive_bench
    common.c
    perf.c
    recursive.c
    recursive_bench.c
)

target_link_libraries(recursive_bench
    ${CHECK_LIBRARIES}
)

add_executable(recursive_test
    common.c
    perf.c
    recursive.c
    recursive_test.c
)

target_link_libraries(recursive_test
    ${CHECK_LIBRARIES}
)

add_test(test_recursive
    ${CMAKE_CURRENT_BINARY_DIR}/recursive_test
)

# vim:set ts=4 sw=4 et:
