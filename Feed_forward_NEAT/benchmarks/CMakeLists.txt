
cmake_minimum_required (VERSION 3.8)

project(ff_neat_benchmark)

set(
    SOURCES
    "benchmark_neat.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
	PUBLIC ../
    PUBLIC ../../Benchmarker/
)

target_link_directories(${PROJECT_NAME}
	PRIVATE ../
	PRIVATE ../../Benchmarker/
)

target_link_libraries(${PROJECT_NAME} PUBLIC
	ff_neat
	benchmarker
)

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
