FetchContent_Declare(
    tinyexpr-git
    GIT_REPOSITORY https://github.com/Blake-Madden/tinyexpr-plusplus.git
    GIT_TAG v1.1.0
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(tinyexpr-git)

add_library(tinyexpr STATIC
    ${tinyexpr-git_SOURCE_DIR}/tinyexpr.cpp
)

list(APPEND DEP_INCLUDES ${tinyexpr-git_SOURCE_DIR})