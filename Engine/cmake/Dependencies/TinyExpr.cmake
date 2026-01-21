FetchContent_Declare(
    tinyexpr-git
    GIT_REPOSITORY https://github.com/Blake-Madden/tinyexpr-plusplus.git
    GIT_TAG v1.1.0
    GIT_PROGRESS   TRUE
)
FetchContent_Populate(tinyexpr-git)

add_library(tinyexpr-cpp STATIC
    ${tinyexpr-git_SOURCE_DIR}/tinyexpr.cpp
)
target_include_directories(tinyexpr-cpp PUBLIC ${tinyexpr-git_SOURCE_DIR})

list(APPEND DEP_INCLUDES ${tinyexpr-git_SOURCE_DIR})