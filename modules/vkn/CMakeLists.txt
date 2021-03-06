# CMake project initialization

cmake_minimum_required(VERSION 3.14...3.17 FATAL_ERROR)

# Set the project language toolchain, version and description

project(vkn
   VERSION 0.0.1
   DESCRIPTION "A small wrapper library to simplify the construction of Vulkan handles"
   LANGUAGES CXX
)

# User interface declarations

option(VERMILLON_VKN_STATIC "Build ${PROJECT_NAME} as a static library" ON)
option(VERMILLON_VKN_BUILD_TESTS "Build ${PROJECT_NAME}'s unit tests" OFF)
option(VERMILLON_VKN_BUILD_BENCH "Build ${PROJECT_NAME}'s performance tests" OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE FORCE)
endif ()

message(STATUS "[${PROJECT_NAME}] ${PROJECT_VERSION}")

if (CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "[${PROJECT_NAME}] Build type: Release")
else ()
    message(STATUS "[${PROJECT_NAME}] Build type: Debug")
endif ()

message(STATUS "[${PROJECT_NAME}] Building as a static library: ${VERMILLON_UTIL_STATIC}")
message(STATUS "[${PROJECT_NAME}] Building unit tests: ${VERMILLON_UTIL_BUILD_TESTS}")
message(STATUS "[${PROJECT_NAME}] Building benchmarks: ${VERMILLON_UTIL_BUILD_BENCH}")

# Define imported targets

include(CMakeLists.txt.in)

# Define exported targets

add_library(${PROJECT_NAME} STATIC)
add_library(vermillon::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if (VERMILLON_VKN_STATIC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE VKN_STATIC) 
endif ()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_EXTENSIONS OFF
)

target_compile_features(${PROJECT_NAME}
    PRIVATE
        cxx_std_20
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        vermillon::util
        Vulkan-Headers
        glslang
        SPIRV
        spirv-cross-core
    PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:DEBUG>>:-lasan>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-lasan>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-lubsan>
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
)

target_compile_options(${PROJECT_NAME}
   PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:DEBUG>>:-o0 -g -Wall -Wextra -Werror -fno-omit-frame-pointer 
        -fsanitize=address>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:RELEASE>>:-o3>

        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-o0 -g -Wall -Wextra -Werror -fno-omit-frame-pointer
        -Wconversion -fsanitize=address -fsanitize=undefined>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:-o3>
)

target_include_directories(${PROJECT_NAME}
    SYSTEM 
        INTERFACE
            Vulkan-Headers
        PUBLIC
            glslang
            SPIRV
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_sources(${PROJECT_NAME}
    PRIVATE
        source/vkn/sync/fence.cpp
        source/vkn/sync/semaphore.cpp
        source/vkn/buffer.cpp
        source/vkn/command_pool.cpp
        source/vkn/core.cpp
        source/vkn/descriptor_pool.cpp
        source/vkn/descriptor_set_layout.cpp
        source/vkn/device.cpp
        source/vkn/framebuffer.cpp
        source/vkn/instance.cpp
        source/vkn/physical_device.cpp
        source/vkn/pipeline.cpp
        source/vkn/render_pass.cpp
        source/vkn/shader.cpp
        source/vkn/swapchain.cpp
)
