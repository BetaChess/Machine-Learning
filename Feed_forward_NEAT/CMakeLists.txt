﻿# CMakeList.txt : CMake project for Machine Learning, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(ff_neat)



set(
    SOURCES
    "NEAT.cpp" 
    "NEAT.h"
    "evaluator.h" 
    "evaluator.cpp"
    "calculator.h" 
    "calculator.cpp"
)

# Add source to this project's executable.
add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC ../Benchmarker/
)

target_link_directories(${PROJECT_NAME}
	PRIVATE ../Benchmarker
)
 
target_link_libraries(${PROJECT_NAME}
	benchmarker
)


set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

message($<TARGET_FILE_DIR:${PROJECT_NAME}>)

# Adding debug PDB
set_property(
    TARGET ${PROJECT_NAME}
    APPEND PROPERTY
        CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)


add_subdirectory("unit_tests")
add_subdirectory("benchmarks")