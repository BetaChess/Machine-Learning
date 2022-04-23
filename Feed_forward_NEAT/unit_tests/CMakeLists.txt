
cmake_minimum_required (VERSION 3.8)

project(ff_neat_unit_tests)

set(
    SOURCES
    "NetworkEvaluationTests.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} 
	PUBLIC ../
    PUBLIC ../../vendor/googletest/googletest/include
)

target_link_directories(${PROJECT_NAME}
	PRIVATE ../
	PRIVATE ../../vendor/googletest/
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    gtest_main
	ff_neat
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

add_test(
	NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
