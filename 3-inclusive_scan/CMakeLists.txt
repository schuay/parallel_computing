cmake_minimum_required(VERSION 2.8.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../CMakeModules/")

project(pc3)

find_package(Check)
find_package(MPI REQUIRED)

set(CMAKE_C_COMPILE_FLAGS ${CMAKE_C_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_C_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} ${MPI_LINK_FLAGS})

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions("-Wall -Wextra -pedantic -std=c99 -D_GNU_SOURCE")
endif()

set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../common")
if(NOT EXISTS "${COMMON_DIR}")
    set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/common")
endif()

set(TEST_SEED 42)

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

add_executable(arrayscan_ref_bench
    ${COMMON_DIR}/csv.c
    bench.c
    arrayscan.c
    arrayscan_ref.c
)

target_link_libraries(arrayscan_ref_bench
    ${MPI_C_LIBRARIES}
)

add_executable(arrayscan_par_mpi_bench
    ${COMMON_DIR}/csv.c
    bench.c
    arrayscan.c
    arrayscan_par.c
    exscan_mpi.c
)

target_link_libraries(arrayscan_par_mpi_bench
    ${MPI_C_LIBRARIES}
)

add_executable(arrayscan_par_hillis-steele_bench
    ${COMMON_DIR}/csv.c
    bench.c
    arrayscan.c
    arrayscan_par.c
    exscan_hillis-steele.c
)

target_link_libraries(arrayscan_par_hillis-steele_bench
    ${MPI_C_LIBRARIES}
)

if(CHECK_FOUND)
    enable_testing()

    include_directories(
        ${CHECK_INCLUDE_DIRS}
    )

    add_executable(arrayscan_par_mpi_test
        arrayscan.c
        arrayscan_par.c
        test.c
        exscan_mpi.c
    )

    target_link_libraries(arrayscan_par_mpi_test
        ${CHECK_LIBRARIES}
        ${MPI_C_LIBRARIES}
    )

    add_test(arrayscan_par_mpi-test
        ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 4 ${CMAKE_CURRENT_BINARY_DIR}/arrayscan_par_mpi_test ${TEST_SEED}
    )

    add_executable(arrayscan_par_hillis-steele_test
        arrayscan.c
        arrayscan_par.c
        test.c
        exscan_hillis-steele.c
    )

    target_link_libraries(arrayscan_par_hillis-steele_test
        ${CHECK_LIBRARIES}
        ${MPI_C_LIBRARIES}
    )

    add_test(arrayscan_par_hillis-steele-test
        ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 4 ${CMAKE_CURRENT_BINARY_DIR}/arrayscan_par_hillis-steele_test ${TEST_SEED}
    )

endif()

# vim:set ts=4 sw=4 et:
