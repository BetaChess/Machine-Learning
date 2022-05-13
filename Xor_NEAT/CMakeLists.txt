# CMakeList.txt : CMake project for Machine Learning, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(xor_neat)

set(
    SOURCES
    "evaluator_xor.h"
	"evaluator_xor.cpp"
)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${SOURCES} "main.cpp" )

set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(${PROJECT_NAME} 
	PUBLIC ../Feed_forward_NEAT/
    PUBLIC ../Benchmarker/
)

target_link_directories(${PROJECT_NAME}
	PRIVATE ../Feed_forward_NEAT
	PRIVATE ../Benchmarker
)
 
target_link_libraries(${PROJECT_NAME}
	ff_neat
	benchmarker
)

# Do the same for the lib version
add_library (${PROJECT_NAME}_lib ${SOURCES} )

set_target_properties(${PROJECT_NAME}_lib 
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(${PROJECT_NAME}_lib  
	PUBLIC ../Feed_forward_NEAT/
    PUBLIC ../Benchmarker/
)

target_link_directories(${PROJECT_NAME}_lib 
	PRIVATE ../Feed_forward_NEAT
	PRIVATE ../Benchmarker
)
 
target_link_libraries(${PROJECT_NAME}_lib 
	ff_neat
	benchmarker
)

#message(${CMAKE_BINARY_DIR}/pdb)

# Adding debug PDB
set_property(
    TARGET ${PROJECT_NAME}
    APPEND PROPERTY
        CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory("unit_tests")