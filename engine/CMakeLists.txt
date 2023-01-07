set(ENGINE_ROOT_DIR "${PICCOLO_ROOT_DIR}/engine")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rdparty")
set(ENGINE_ASSET_DIR "/asset")

set(DEPLOY_CONFIG_DIR "configs/deployment")
set(DEVELOP_CONFIG_DIR "configs/development")

option(ENABLE_PHYSICS_DEBUG_RENDERER "Enable Physics Debug Renderer" OFF)

# only support physics debug render at windows platform
if(NOT WIN32)
  if(ENABLE_PHYSICS_DEBUG_RENDERER)
    message(WARNING "Disable Physics Debug Renderer")
    set(ENABLE_PHYSICS_DEBUG_RENDERER OFF CACHE BOOL "" FORCE)
  endif()
endif()

if(ENABLE_PHYSICS_DEBUG_RENDERER)
  set(JOLT_ASSET_DIR "/jolt-asset")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PiccoloEditor)
endif()

set(glslangValidator_executable glslang::glslangValidator)

# if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/asset/)
# file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/asset/" DESTINATION "${CMAKE_BINARY_DIR}/asset")
# endif()

# if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/shader/glsl/)
# file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shader/glsl/" DESTINATION "${CMAKE_BINARY_DIR}/shader/glsl/")
# endif()

# if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/shader/include/)
# file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shader/include/" DESTINATION "${CMAKE_BINARY_DIR}/shader/include/")
# endif()

# if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/template/)
# file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/template/" DESTINATION "${CMAKE_BINARY_DIR}/template")
# endif()

set(SHADER_COMPILE_TARGET PiccoloShaderCompile)
add_subdirectory(shader)

add_subdirectory(3rdparty)

add_subdirectory(source/meta_parser)
add_subdirectory(source/runtime)
add_subdirectory(source/editor)
# add_subdirectory(source/playground)
# add_subdirectory(source/test)

set(CODEGEN_TARGET "PiccoloPreCompile")
include(source/precompile/precompile.cmake)
set_target_properties("${CODEGEN_TARGET}" PROPERTIES FOLDER "Engine" )

add_dependencies("${CODEGEN_TARGET}" "PiccoloParser")
