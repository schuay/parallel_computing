cmake_minimum_required(VERSION 2.8.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../CMakeModules/")

project(pc1)

find_package(Check)
find_package(OpenMP REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions("-Wall -Wextra -pedantic -std=c99")
endif()

include_directories(
    ../common
)

add_executable(merge_bench
    ../common/perf.c
    merge.c
    merge_par.c
    bench.c
)

if(CHECK_FOUND)
    enable_testing()

    include_directories(
        ${CHECK_INCLUDE_DIRS}
    )

    add_executable(merge_test
        merge.c
        merge_par.c
        test.c
    )

    target_link_libraries(merge_test
        ${CHECK_LIBRARIES}
    )

    add_test(test_merge
        ${CMAKE_CURRENT_BINARY_DIR}/merge_test
    )
endif()

# vim:set ts=4 sw=4 et:
