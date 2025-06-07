cmake_minimum_required(VERSION 3.15)
project(RayEngine)

if (UNIX)
  set(COMPILE_PLATFORM linux_amd64)
  set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
  set(COMPILE_PLATFORM_PATH ${COMPILE_PLATFORM}_${BUILD_TYPE})
endif (UNIX)

if (MSVC)
  set(COMPILE_PLATFORM win64)
endif (MSVC)

set(FETCHCONTENT_QUIET OFF)
include(FetchContent)

include(${CMAKE_SOURCE_DIR}/cmake/Raylib.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Tracy.cmake)

# Projects
add_subdirectory(source/ImGui)
add_subdirectory(source/Utility)
add_subdirectory(source/Core)
add_subdirectory(source/Rendering)
add_subdirectory(source/Engine)
add_subdirectory(source/Game)
add_subdirectory(content)
