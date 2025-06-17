set(MINIAUDIO_VERSION 0.11.22)
find_package(miniaudio ${MINIAUDIO_VERSION} QUIET)
FetchContent_Declare(
  miniaudio
  DOWNLOAD_EXTRACT_TIMESTAMP OFF
  URL https://github.com/mackron/miniaudio/archive/refs/tags/${MINIAUDIO_VERSION}.tar.gz
)
set(MINIAUDIO_NO_ENGINE ON)
set(MINIAUDIO_NO_NODE_GRAPH ON)
set(MINIAUDIO_NO_EXTRA_NODES ON)
FetchContent_MakeAvailable(miniaudio)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE dl pthread m atomic)
    # Note that GCC and Clang require -msse2, -mavx2, etc. for SIMD optimizations. 
endif()