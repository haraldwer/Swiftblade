FetchContent_Declare(
    rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG        master
    GIT_PROGRESS   TRUE
    GIT_SHALLOW TRUE
    GIT_SUBMODULES_RECURSE FALSE
)

FetchContent_GetProperties(rapidjson)
if(NOT rapidjson_POPULATED)
    FetchContent_Populate(rapidjson)
endif()

list(APPEND DEP_INCLUDES ${rapidjson_SOURCE_DIR}/include)