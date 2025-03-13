cmake_minimum_required(VERSION 3.15)
project(RayEngine)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Raylib
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    set(SUPPORT_CAMERA_SYSTEM OFF) # no camera
    set(OPENGL_VERSION OFF) # OpenGL version
    set(SUPPORT_DEFAULT_FONT OFF) # skip font
    set(SUPPORT_GESTURES_SYSTEM OFF) # skip gestures
    set(SUPPORT_MOUSE_GESTURES OFF) # skip gestures
  endif()
endif()

set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

# Projects
add_subdirectory(source/ImGui)
add_subdirectory(source/Utility)
add_subdirectory(source/Core)
add_subdirectory(source/Rendering)
add_subdirectory(source/Engine)
add_subdirectory(source/Game)
add_subdirectory(content)
