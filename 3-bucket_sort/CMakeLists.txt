cmake_minimum_required(VERSION 2.8.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../CMakeModules/")

project(pc3)

find_package(Check)
find_package(MPI REQUIRED)

set(CMAKE_C_COMPILE_FLAGS ${CMAKE_C_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_C_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} ${MPI_LINK_FLAGS})

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions("-Wall -Wextra -pedantic -std=c99")
endif()

set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../common")
if(NOT EXISTS "${COMMON_DIR}")
    set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/common")
endif()

# Saturn still has old an FindMPI.cmake version without
# MPI_C_LIBRARIES. Hack in a fix here:
if(NOT DEFINED MPI_C_LIBRARIES)
    set(MPI_C_LIBRARIES
        /usr/lib/libmpich.so
        /usr/lib/libopa.a
        /usr/lib/libpthread.so
        /usr/lib/librt.so)
endif()

include_directories(
    ${COMMON_DIR}
    ${MPI_INCLUDE_PATH}
)

add_executable(bucket_ref
    ${COMMON_DIR}/csv.c
    bucket.c
    bucket_ref.c
)

target_link_libraries(bucket_ref
    ${MPI_C_LIBRARIES}
)

add_executable(bucket_bench
    bench.c
    bucket.c
    ${COMMON_DIR}/csv.c
)

target_link_libraries(bucket_bench
    ${MPI_C_LIBRARIES}
)

if(CHECK_FOUND)
    enable_testing()

    include_directories(
        ${CHECK_INCLUDE_DIRS}
    )

    add_executable(bucket_test
        bucket.c
        test.c
    )

    target_link_libraries(bucket_test
        ${CHECK_LIBRARIES}
        ${MPI_C_LIBRARIES}
    )

    add_test(bucket-test
        ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 4 ${CMAKE_CURRENT_BINARY_DIR}/bucket_test
    )
endif()

# vim:set ts=4 sw=4 et:
