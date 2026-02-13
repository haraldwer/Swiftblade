FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
    GIT_PROGRESS   TRUE
    GIT_SHALLOW TRUE
    GIT_SUBMODULES_RECURSE FALSE
)

FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif()

list(APPEND DEP_INCLUDES ${stb_SOURCE_DIR})