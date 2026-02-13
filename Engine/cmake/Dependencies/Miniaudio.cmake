FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG        master
    GIT_PROGRESS   TRUE
    GIT_SHALLOW TRUE
    GIT_SUBMODULES_RECURSE FALSE
)
FetchContent_MakeAvailable(miniaudio)