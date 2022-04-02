# CMakeList.txt : CMake project for Machine Learning, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(benchmarker)

set(
    SOURCES
    "benchmarker.h"
	"benchmarker.cpp"
)

# Add source to this project's executable.
add_library (${PROJECT_NAME} ${SOURCES} )



set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

# Adding debug PDB
set_property(
    TARGET ${PROJECT_NAME}
    APPEND PROPERTY
        CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
