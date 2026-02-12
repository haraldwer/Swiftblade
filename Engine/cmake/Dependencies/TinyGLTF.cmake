
set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)

FetchContent_Declare(
    tinygltf
    GIT_REPOSITORY https://github.com/syoyo/tinygltf
    GIT_TAG        release
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(tinygltf)

list(APPEND DEP_INCLUDES ${tinygltf_SOURCE_DIR}/include)