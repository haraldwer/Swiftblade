FetchContent_Declare(
    picosha2
    GIT_REPOSITORY https://github.com/okdshin/PicoSHA2.git
    GIT_TAG        v1.0.1
    GIT_PROGRESS   TRUE
)

FetchContent_GetProperties(picosha2)
if(NOT picosha2_POPULATED)
    FetchContent_Populate(picosha2)
endif()

list(APPEND DEP_INCLUDES ${picosha2_SOURCE_DIR})