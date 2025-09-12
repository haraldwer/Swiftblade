cmake_minimum_required(VERSION 3.15)
project(RayEngine)

message("Platform: ${PLATFORM}")

if (UNIX)
  set(COMPILE_PLATFORM linux_amd64)
  set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
  set(COMPILE_PLATFORM_PATH ${COMPILE_PLATFORM}_${BUILD_TYPE})
  set(DISABLE_PRECOMPILE_HEADERS ON)
endif (UNIX)

if (MSVC)
  set(COMPILE_PLATFORM win64)
endif (MSVC)

if (EMSCRIPTEN)
  message("Emscripten detected")
  add_link_options(-sUSE_GLFW=3 -sFULL_ES3 -sASYNCIFY -sFORCE_FILESYSTEM=1 -sWASM=2 -sGL_ENABLE_GET_PROC_ADDRESS=1 -sEXPORTED_RUNTIME_METHODS=ccall -sTOTAL_MEMORY=67108864)
  add_link_options(-sASSERTIONS=1 -sASYNCIFY -sASYNCIFY_DEBUG=2 -sSYSCALL_DEBUG=1 -sLIBRARY_DEBUG=1 -sPTHREADS_DEBUG=1 -sGL_DEBUG=1)
  add_link_options(--shell-file ${CMAKE_SOURCE_DIR}/shell.html --profiling --threadprofiler --source-map-base)
  add_compile_options(-pthread)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  set(COMPILE_PLATFORM emscripten)
  set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
  set(COMPILE_PLATFORM_PATH ${COMPILE_PLATFORM}_${BUILD_TYPE})
else ()
  add_compile_definitions(TRACY_ENABLE)
  add_compile_definitions(NAKAMA_ENABLE)
endif ()



set(FETCHCONTENT_QUIET OFF)
include(FetchContent)

include(${CMAKE_SOURCE_DIR}/cmake/Raylib.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Tracy.cmake)

# Projects
add_subdirectory(source/ImGui)
add_subdirectory(source/Utility)
add_subdirectory(source/Core)
add_subdirectory(source/Rendering)
add_subdirectory(source/Music)
add_subdirectory(source/Engine)
add_subdirectory(source/Game)
add_subdirectory(content)
