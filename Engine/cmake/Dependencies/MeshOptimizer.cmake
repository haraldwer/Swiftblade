FetchContent_Declare(
    meshoptimizer
    GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git
    GIT_TAG        master
    GIT_PROGRESS   TRUE
    GIT_SHALLOW TRUE
    GIT_SUBMODULES_RECURSE FALSE
)
FetchContent_MakeAvailable(meshoptimizer)
