FetchContent_Declare(
    fastnoiselite
    GIT_REPOSITORY https://github.com/Auburn/FastNoiseLite.git
    GIT_TAG        master
    GIT_PROGRESS   TRUE
)

FetchContent_GetProperties(fastnoiselite)
if(NOT fastnoiselite_POPULATED)
    FetchContent_MakeAvailable(fastnoiselite)
endif()

list(APPEND DEP_INCLUDES ${fastnoiselite_SOURCE_DIR}/Cpp)