# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Machine Learning")

if (WIN32)
	add_compile_definitions("PLATFORM_WINDOWS")
endif()

if (DEBUGWITHINFO)
	add_compile_definitions("DEBUGWITHINFO")
	add_compile_definitions("DEBUGADDITIONAL")
endif()

if (DEBUGADDITIONAL)
	add_compile_definitions("DEBUGADDITIONAL")
endif()


# Include sub-projects.
add_subdirectory ("Feed_forward_NEAT")
add_subdirectory ("Benchmarker")
add_subdirectory ("Number_recognizer")
add_subdirectory ("Xor_NEAT")
add_subdirectory ("vendor/glfw")
add_subdirectory ("vendor/glm")