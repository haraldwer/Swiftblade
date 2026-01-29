FetchContent_Declare(
    tiny_aes
    GIT_REPOSITORY https://github.com/kokke/tiny-AES-c.git
    GIT_TAG master
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(tiny_aes)

list(APPEND DEP_INCLUDES ${tiny_aes_SOURCE_DIR})