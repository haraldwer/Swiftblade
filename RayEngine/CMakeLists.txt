cmake_minimum_required(VERSION 3.5) # FetchContent is available in 3.11+
project(RayEngine)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.0)
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

# Source files
file(GLOB_RECURSE sources
  "${PROJECT_SOURCE_DIR}/source/*.h"
  "${PROJECT_SOURCE_DIR}/source/*.cpp"
  "${PROJECT_SOURCE_DIR}/source/*.hpp"
  "${PROJECT_SOURCE_DIR}/content/*.vs"
  "${PROJECT_SOURCE_DIR}/content/*.fs"
  "${PROJECT_SOURCE_DIR}/content/*.json"
)
add_executable(${PROJECT_NAME} ${sources})

# Includes
target_include_directories(${PROJECT_NAME} 
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/physx
    ${PROJECT_SOURCE_DIR}/source
)

# Pch
target_precompile_headers(${PROJECT_NAME} 
  PRIVATE
    "${PROJECT_SOURCE_DIR}/source/pch.h"
)

# Linking
file(GLOB_RECURSE libs "${PROJECT_SOURCE_DIR}/library/*.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE raylib ${libs})

# Compile options
set_target_properties(${PROJECT_NAME} 
  PROPERTIES 
    CXX_STANDARD 20
    UNITY_BUILD_MODE BATCH
    UNITY_BUILD_BATCH_SIZE 10)
target_compile_options(${PROJECT_NAME} PUBLIC /MP) # Multithreading

# Copy DLL files to exe folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/library/dll/ $<TARGET_FILE_DIR:${PROJECT_NAME}>)

# -- #
