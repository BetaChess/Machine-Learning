# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Machine Learning")

enable_testing()

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

if (BENCHMARK)
	add_compile_definitions("BENCHMARK_ENABLED")
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_compile_options("$<$<NOT:$<CONFIG:Debug>>:/Zi>")
add_link_options("$<$<NOT:$<CONFIG:Debug>>:/DEBUG>")
add_link_options("$<$<NOT:$<CONFIG:Debug>>:/OPT:REF>")
add_link_options("$<$<NOT:$<CONFIG:Debug>>:/OPT:ICF>")

# Include sub-projects.
add_subdirectory ("Feed_forward_NEAT")
add_subdirectory ("Benchmarker")
add_subdirectory ("Number_recognizer")
add_subdirectory ("Xor_NEAT")
add_subdirectory ("Asexual_reproduction_simulation")
add_subdirectory ("vendor/glfw")
add_subdirectory ("vendor/glm")
add_subdirectory ("vendor/googletest")