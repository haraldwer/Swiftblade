FetchContent_Declare(
    tinyexpr-git
    GIT_REPOSITORY  https://github.com/Blake-Madden/tinyexpr-plusplus.git
    GIT_TAG         v1.1.0
    GIT_PROGRESS    TRUE
)
FetchContent_GetProperties(tinyexpr-git)
if(NOT tinyexpr-git_POPULATED)
    FetchContent_Populate(tinyexpr-git)
endif()

list(APPEND DEP_INCLUDES ${tinyexpr-git_SOURCE_DIR})