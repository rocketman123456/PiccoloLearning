set(third_party_folder "ThirdParty")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(NOT TARGET imgui)
    include(imgui.cmake)
    set_target_properties(imgui PROPERTIES FOLDER ${third_party_folder}/imgui)
endif()

if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
    set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder}/glfw)
    set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder}/glfw)
endif()

if(NOT TARGET spdlog)
    option(SPDLOG_BUILD_EXAMPLE "" OFF)
    option(SPDLOG_INSTALL "" OFF)
    add_subdirectory(spdlog)
    set_target_properties(spdlog PROPERTIES FOLDER ${third_party_folder}/spdlog)
endif()

if(NOT TARGET stb)
    include(stb.cmake)
    set_target_properties(stb PROPERTIES FOLDER ${third_party_folder}/stb)
endif()

if(NOT TARGET eigen)
    option(BUILD_TESTING "" OFF)
    option(EIGEN_TEST_CXX11 "" OFF)
    option(EIGEN_SPLIT_LARGE_TESTS "" OFF)
    add_subdirectory(eigen)
    set_target_properties(eigen PROPERTIES FOLDER ${third_party_folder}/eigen)
    set_target_properties(uninstall PROPERTIES FOLDER ${third_party_folder}/eigen)
endif()

if(NOT TARGET tinyobjloader)
    add_subdirectory(tinyobjloader)
    set_target_properties(tinyobjloader PROPERTIES FOLDER ${third_party_folder}/tinyobjloader)
    set_target_properties(uninstall PROPERTIES FOLDER ${third_party_folder}/tinyobjloader)
endif()

if(NOT TARGET tinygltf)
    option(TINYGLTF_BUILD_LOADER_EXAMPLE "" OFF)
    option(TINYGLTF_INSTALL "" OFF)
    add_subdirectory(tinygltf)
    set_target_properties(tinygltf PROPERTIES FOLDER ${third_party_folder}/tinygltf)
    set_target_properties(uninstall PROPERTIES FOLDER ${third_party_folder}/tinygltf)
endif()

if(NOT TARGET Jolt)
    option(TARGET_HELLO_WORLD "" OFF)
    option(TARGET_PERFORMANCE_TEST "" OFF)
    option(TARGET_SAMPLES "" OFF)
    option(TARGET_UNIT_TESTS "" OFF)
    option(TARGET_VIEWER "" OFF)
    # option(CROSS_PLATFORM_DETERMINISTIC "" OFF)

    if(ENABLE_PHYSICS_DEBUG_RENDERER)
        option(TARGET_TEST_FRAMEWORK "" ON)
    else()
        option(TARGET_TEST_FRAMEWORK "" OFF)
    endif()

    add_subdirectory(JoltPhysics/Build)

    target_compile_definitions(Jolt PUBLIC JPH_DISABLE_CUSTOM_ALLOCATOR)

    if(ENABLE_PHYSICS_DEBUG_RENDERER)
        set_target_properties(Jolt TestFramework
            PROPERTIES 
            FOLDER ${third_party_folder}/JoltPhysics
            MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    else()
        set_target_properties(Jolt
            PROPERTIES 
            FOLDER ${third_party_folder}/JoltPhysics
            MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    endif()

endif()

if(NOT TARGET sol2)
    add_subdirectory(sol2-3.3.0)
endif()

if(NOT TARGET volk)
    add_subdirectory(volk)
endif()

if(NOT TARGET glslang)
    option(SKIP_GLSLANG_INSTALL "" ON)
    option(ENABLE_HLSL "" ON)
    option(ENABLE_OPT "" ON)
    add_subdirectory(glslang-sdk)
    set_target_properties(glslang PROPERTIES FOLDER ${third_party_folder}/glslang-sdk)
endif()

if(NOT TARGET lua)
    include(lua.cmake)
endif()

if(NOT TARGET json11)
    add_subdirectory(json11)
    set_target_properties(json11 PROPERTIES FOLDER ${third_party_folder}/json11)
endif()

if(NOT TARGET soloud)
    include(soloud.cmake)
endif()

if(NOT TARGET lz4)
    option(LZ4_BUILD_CLI "" OFF)
    option(LZ4_BUILD_LEGACY_LZ4C "" OFF)
    option(BUILD_SHARED_LIBS "" OFF)
    add_subdirectory(lz4/build/cmake)
endif()

if(NOT TARGET zip)
    option(BUILD_EXAMPLES "" OFF)
    option(BUILD_TOOLS "" OFF)
    option(BUILD_DOC "" OFF)
    option(BUILD_REGRESS "" OFF)
    option(BUILD_SHARED_LIBS "" OFF)
    add_subdirectory(libzip)
endif()
