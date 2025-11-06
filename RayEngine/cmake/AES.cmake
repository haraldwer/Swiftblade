
FetchContent_Declare(
    tiny_aes_c
    GIT_REPOSITORY https://github.com/kokke/tiny-AES-c.git
    GIT_TAG master
)

FetchContent_GetProperties(tiny_aes_c)
if(NOT tiny_aes_c_POPULATED)
    FetchContent_MakeAvailable(tiny_aes_c)
    add_library(tiny_aes_c STATIC ${tiny_aes_c_SOURCE_DIR}/aes.c )
    target_include_directories(tiny_aes_c PUBLIC ${tiny_aes_c_SOURCE_DIR})
endif()