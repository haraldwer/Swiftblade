
# Dependencies
if (EMSCRIPTEN)

    add_library(glfw INTERFACE)
    add_library(webgpu INTERFACE)
    target_link_options(webgpu INTERFACE -sUSE_WEBGPU=1)
    target_link_options(glfw INTERFACE -sUSE_GLFW=3)
    function(target_copy_webgpu_binaries)
    endfunction()

else (EMSCRIPTEN)

    set(WEBGPU_BACKEND WGPU CACHE BOOL "" FORCE)
    set(WEBGPU_BUILD_FROM_SOURCE OFF CACHE BOOL "" FORCE)
    set(WEBGPU_LINK_TYPE STATIC CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        glfw-light
        URL      https://eliemichel.github.io/LearnWebGPU/_downloads/6873a344e35ea9f5e4fc7e5cc85d3ab8/glfw-3.4.0-light.zip
        URL_HASH MD5=e6684142080457e9227e4f78a6f84fc2
        GIT_PROGRESS   TRUE
    )
    FetchContent_Declare(
        glfw3webgpu
        URL https://github.com/eliemichel/glfw3webgpu/releases/download/v1.3.0-alpha/glfw3webgpu-v1.3.0-alpha.zip
        URL_HASH MD5=7742bdfe31d08371ab5560ba9684922a
        GIT_PROGRESS   TRUE
    )
    FetchContent_Declare(
        webgpu
        URL https://github.com/eliemichel/WebGPU-distribution/archive/refs/tags/main-v0.3.0-alpha.zip
        URL_HASH MD5=ea1195dc6c7661c36aa13ea5b734b86e
        GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(glfw-light glfw3webgpu webgpu)

endif (EMSCRIPTEN)