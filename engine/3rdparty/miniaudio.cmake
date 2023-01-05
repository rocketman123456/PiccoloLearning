file(GLOB miniaudio_sources CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/miniaudio/miniaudio.h")
add_library(miniaudio INTERFACE ${miniaudio_sources})
target_include_directories(miniaudio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/miniaudio)
