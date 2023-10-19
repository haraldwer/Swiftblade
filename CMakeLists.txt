cmake_minimum_required(VERSION 3.5) # FetchContent is available in 3.11+
project(RayEngine)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 4.5.0)
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
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()




# -- Project -- #

file(GLOB_RECURSE sources
  "${PROJECT_SOURCE_DIR}/source/*.h"
  "${PROJECT_SOURCE_DIR}/source/*.cpp"
  "${PROJECT_SOURCE_DIR}/source/*.hpp"
)
add_executable(${PROJECT_NAME} ${sources})

target_include_directories(${PROJECT_NAME} 
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/source
)

target_precompile_headers(${PROJECT_NAME} 
  PUBLIC
    "${PROJECT_SOURCE_DIR}/source/pch.h"
)

target_link_libraries(${PROJECT_NAME} raylib)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

# -- #




# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()