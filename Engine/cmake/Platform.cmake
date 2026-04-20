message("-- Setting platform-specific configuration: " ${PROJECT_NAME})

# Platform specific

if (UNIX)
    set(COMPILE_PLATFORM linux_amd64)
    set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
    set(COMPILE_PLATFORM_PATH ${COMPILE_PLATFORM}_${BUILD_TYPE})
endif (UNIX)

if (MSVC OR MINGW)
    set(COMPILE_PLATFORM win64)
    add_compile_definitions(_WINDOWS)
endif (MSVC OR MINGW)

if (EMSCRIPTEN)
    message("-- Emscripten detected")
    # Default options
    #add_link_options(-sUSE_GLFW=3 -sFULL_ES3 -sGL_ENABLE_GET_PROC_ADDRESS=1 -sEXPORTED_RUNTIME_METHODS=ccall,HEAPF32 )
    add_link_options(-sALLOW_MEMORY_GROWTH=1 -sFORCE_FILESYSTEM=1 -sTOTAL_MEMORY=93061120 -sWASM=1)
    # Async options
    add_link_options(-sASYNCIFY) # TODO: Investigate -sASYNCIFY_ONLY
    # Thread support
    add_compile_options(-pthread)
    add_link_options(-pthread)
    add_link_options(-sPTHREAD_POOL_SIZE=4)
    # Debug options
    if (BUILD_TYPE MATCHES Debug)
        message("-- Debug Emscripten build")
        add_link_options(-sASSERTIONS=2 -sSTACK_OVERFLOW_CHECK=2)
        add_link_options(-sPTHREADS_DEBUG=1 -sSOCKET_DEBUG=1 -sWEBSOCKET_DEBUG=1 -sFS_DEBUG=1 -sLIBRARY_DEBUG=0 -sSYSCALL_DEBUG=0 -sASYNCIFY_DEBUG=0)
        #add_link_options(--profiling --threadprofiler)
    else ()
        #add_link_options(--no-wasm-opt)
    endif ()
    # Shell
    add_link_options(--shell-file ${CMAKE_SOURCE_DIR}/shell_minimal.html)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(COMPILE_PLATFORM emscripten)
    set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
    set(COMPILE_PLATFORM_PATH ${COMPILE_PLATFORM}_${BUILD_TYPE})
else ()
    add_compile_definitions(NAKAMA_ENABLE)
endif ()
