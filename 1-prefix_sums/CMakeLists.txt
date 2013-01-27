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

set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../common")
if(NOT EXISTS "${COMMON_DIR}")
    set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/common")
endif()

include_directories(
    ${COMMON_DIR}
)

add_executable(reference_bench
    common.c
    ${COMMON_DIR}/perf.c
    reference.c
    ${COMMON_DIR}/csv.c
    bench.c
)
SET_TARGET_PROPERTIES(reference_bench PROPERTIES COMPILE_DEFINITIONS "NOPERF")

add_executable(recursive_bench
    common.c
    ${COMMON_DIR}/perf.c
    recursive.c
    ${COMMON_DIR}/csv.c
    bench.c
)
SET_TARGET_PROPERTIES(recursive_bench PROPERTIES COMPILE_DEFINITIONS "NOPERF")

add_executable(iterative_bench
    common.c
    ${COMMON_DIR}/perf.c
    iterative.c
    ${COMMON_DIR}/csv.c
    bench.c
)
SET_TARGET_PROPERTIES(iterative_bench PROPERTIES COMPILE_DEFINITIONS "NOPERF")

add_executable(hillis-steele_bench
    common.c
    ${COMMON_DIR}/perf.c
    hillis-steele.c
    ${COMMON_DIR}/csv.c
    bench.c
)
SET_TARGET_PROPERTIES(hillis-steele_bench PROPERTIES COMPILE_DEFINITIONS "NOPERF")

add_executable(reduction_bench
    common.c
    ${COMMON_DIR}/perf.c
    reduction.c
    ${COMMON_DIR}/csv.c
    bench.c
)
SET_TARGET_PROPERTIES(reduction_bench PROPERTIES COMPILE_DEFINITIONS "NOPERF")

add_executable(recursive_work
    common.c
    ${COMMON_DIR}/perf.c
    recursive.c
    ${COMMON_DIR}/csv.c
    work.c
)

add_executable(iterative_work
    common.c
    ${COMMON_DIR}/perf.c
    iterative.c
    ${COMMON_DIR}/csv.c
    work.c
)

add_executable(hillis-steele_work
    common.c
    ${COMMON_DIR}/perf.c
    hillis-steele.c
    ${COMMON_DIR}/csv.c
    work.c
)

add_executable(reduction_work
    common.c
    ${COMMON_DIR}/perf.c
    reduction.c
    ${COMMON_DIR}/csv.c
    work.c
)

if(CHECK_FOUND)
    enable_testing()

    include_directories(
        ${CHECK_INCLUDE_DIRS}
    )

    add_executable(recursive_test
        common.c
        ${COMMON_DIR}/perf.c
        recursive.c
        test.c
    )

    target_link_libraries(recursive_test
        ${CHECK_LIBRARIES}
    )

    add_test(test_recursive
        ${CMAKE_CURRENT_BINARY_DIR}/recursive_test
    )

    add_executable(iterative_test
        common.c
        ${COMMON_DIR}/perf.c
        iterative.c
        test.c
    )

    target_link_libraries(iterative_test
        ${CHECK_LIBRARIES}
    )

    add_test(test_iterative
        ${CMAKE_CURRENT_BINARY_DIR}/iterative_test
    )

    add_executable(hillis-steele_test
        common.c
        ${COMMON_DIR}/perf.c
        hillis-steele.c
        test.c
    )

    target_link_libraries(hillis-steele_test
        ${CHECK_LIBRARIES}
    )

    add_test(test_hillis-steele
        ${CMAKE_CURRENT_BINARY_DIR}/hillis-steele_test
    )
endif()

# vim:set ts=4 sw=4 et:
