set(third_party_folder "ThirdParty")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(NOT TARGET imgui)
    include(imgui.cmake)
endif()

if(NOT TARGET Jolt)
    include(jolt.cmake)
endif()

if(NOT TARGET soloud)
    include(soloud.cmake)
endif()
