cmake_minimum_required(VERSION 3.20)
project(Counting_non_matching_characters)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
ENDIF()

find_package(MPI REQUIRED)

add_executable(Counting_non_matching_characters main.cpp)

target_link_libraries(Counting_non_matching_characters PUBLIC MPI::MPI_CXX)