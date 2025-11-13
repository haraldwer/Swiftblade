
FetchContent_Declare(
    tiny_aes
    GIT_REPOSITORY https://github.com/kokke/tiny-AES-c.git
    GIT_TAG master
)

FetchContent_GetProperties(tiny_aes)
if(NOT tiny_aes_POPULATED)
    FetchContent_MakeAvailable(tiny_aes)
    add_library(tiny_aes STATIC ${tiny_aes_SOURCE_DIR}/aes.c )
    target_include_directories(tiny_aes PUBLIC ${tiny_aes_SOURCE_DIR})
endif()