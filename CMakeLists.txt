cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(Piccolo VERSION 0.1.0)

if(CMAKE_BUILD_TYPE)
  message("Build Type ${CMAKE_BUILD_TYPE}")
else()
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

include(CMakeDependentOption)

find_package(LLVM CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
find_package(tinyobjloader CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(libzip CONFIG REQUIRED)
find_package(lz4 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
find_package(unofficial-vulkan-memory-allocator CONFIG REQUIRED)

find_path(JSON11_INCLUDE_DIRS "json11.hpp")
set(JSON11_LIB_DEBUG_DIRS "${JSON11_INCLUDE_DIRS}/../debug/lib")
set(JSON11_LIB_DIRS "${JSON11_INCLUDE_DIRS}/../lib")

find_path(JOLTPHYSICS_INCLUDE_DIRS "Jolt/Jolt.h")
set(JOLTPHYSICS_LIB_DEBUG_DIRS "${JOLTPHYSICS_INCLUDE_DIRS}/../debug/lib")
set(JOLTPHYSICS_LIB_DIRS "${JOLTPHYSICS_INCLUDE_DIRS}/../lib")

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

set(PICCOLO_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_INSTALL_PREFIX "${PICCOLO_ROOT_DIR}/build/bin")
set(BINARY_ROOT_DIR "${PICCOLO_ROOT_DIR}/bin")

add_subdirectory(engine)
