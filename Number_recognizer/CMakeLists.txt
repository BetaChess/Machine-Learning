﻿# CMakeList.txt : CMake project for Machine Learning, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(Number_recognizer)

set(
    SOURCES
    "Number_recognizer.cpp"
    "phm_window.h" 
    "phm_window.cpp" 
    "phm_swapchain.h" 
    "phm_swapchain.cpp" 
    "phm_app.h" 
    "phm_app.cpp" 
    "phm_pipeline.h" 
    "phm_pipeline.cpp" 
    "phm_device.h" 
    "phm_device.cpp" 
    "phm_model.h" 
    "phm_model.cpp" 
    "phm_object.h" 
    "phm_object.cpp"
    "phm_renderer.h" 
    "phm_renderer.cpp" 
    "simple_render_system.h" 
    "simple_render_system.cpp"
    "phm_buffer.cpp" 
    "phm_buffer.h" 
    "phm_descriptor.cpp" 
    "phm_descriptor.h" 
    "phm_keyboardController.cpp" 
    "phm_keyboardController.h"
    "pch.h"
    "phm_utils.h"
    "image_data.h" 
    "image_data.cpp" 
    "phm_utils.cpp"
    "time.cpp" 
    "time.h" 
)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${SOURCES} )

# Add source groups
source_group("Render Systems" FILES 
	"simple_render_system.cpp"
	"simple_render_system.h"
)

source_group("Input" FILES
    "phm_keyboardController.cpp"
    "phm_keyboardController.h"
)

source_group("Vulkan" FILES
    "phm_swapchain.h" 
    "phm_swapchain.cpp" 
    "phm_pipeline.h" 
    "phm_pipeline.cpp" 
    "phm_device.h" 
    "phm_device.cpp" 
    "phm_buffer.h" 
    "phm_buffer.cpp"  
    "phm_descriptor.h" 
    "phm_descriptor.cpp"
)

source_group("Engine" FILES
    "phm_model.cpp"
    "phm_model.h"
    "phm_object.cpp"
    "phm_object.h"
    "phm_renderer.cpp"
    "phm_renderer.h"
)

set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(${PROJECT_NAME} 
	PUBLIC $ENV{VULKAN_SDK}/Include/ 
	PUBLIC ../vendor/glfw/include/
    PUBLIC ../vendor/glm/
    PUBLIC ../vendor/tinyobjloader/
	PUBLIC ../Feed_forward_NEAT/
)

target_link_directories(${PROJECT_NAME}
	PRIVATE ../vendor/glfw/src
    PRIVATE $ENV{VULKAN_SDK}/Lib
	PRIVATE ../Feed_forward_NEAT
)
 
target_link_libraries(${PROJECT_NAME}
	glfw
    vulkan-1
	ff_neat
)

# Adding debug PDB
set_property(
    TARGET ${PROJECT_NAME}
    APPEND PROPERTY
        CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

# Precompiled header
target_precompile_headers(${PROJECT_NAME}
    PUBLIC "pch.h"
)


############## Build SHADERS #######################
 
# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS 
  ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} 
  /usr/bin 
  /usr/local/bin 
  ${VULKAN_SDK_PATH}/Bin
  ${VULKAN_SDK_PATH}/Bin32
  $ENV{VULKAN_SDK}/Bin/ 
  $ENV{VULKAN_SDK}/Bin32/
)
 
# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)
 
foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)
 
add_custom_target(
    ${PROJECT_NAME}_Shaders
    SOURCES ${GLSL_SOURCE_FILES}
    DEPENDS ${SPIRV_BINARY_FILES}
)

add_custom_command(
    TARGET ${PROJECT_NAME}_Shaders POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory              
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders
    COMMENT "Copying shaders" VERBATIM 
    )

ADD_DEPENDENCIES(${PROJECT_NAME} ${PROJECT_NAME}_Shaders)


## Copy data to output
# test data
set(
    TESTDATA
    "test_data/t10k-images.idx3-ubyte"
    "test_data/t10k-labels.idx1-ubyte"
)

add_custom_target(
    ${PROJECT_NAME}_testdata
    DEPENDS ${TESTDATA}
)

add_custom_command(
    TARGET ${PROJECT_NAME}_testdata POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory              
        ${CMAKE_CURRENT_SOURCE_DIR}/test_data ${CMAKE_CURRENT_BINARY_DIR}/test_data
    COMMENT "Copying test data" VERBATIM 
    )

ADD_DEPENDENCIES(${PROJECT_NAME} ${PROJECT_NAME}_testdata)

# training data
set(
    TRAININGDATA
    "training_data/train-images.idx3-ubyte"
    "training_data/train-labels.idx1-ubyte"
)

add_custom_target(
    ${PROJECT_NAME}_trainingdata
    DEPENDS ${TESTDATA}
)

add_custom_command(
    TARGET ${PROJECT_NAME}_trainingdata POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory              
        ${CMAKE_CURRENT_SOURCE_DIR}/training_data ${CMAKE_CURRENT_BINARY_DIR}/training_data
    COMMENT "Copying training data" VERBATIM 
    )

ADD_DEPENDENCIES(${PROJECT_NAME} ${PROJECT_NAME}_trainingdata)