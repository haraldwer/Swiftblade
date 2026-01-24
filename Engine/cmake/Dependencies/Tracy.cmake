if (NOT RETAIL_MODE)

    set(TRACY_ENABLE ON CACHE BOOL "" FORCE)
    set(TRACY_ON_DEMAND ON CACHE BOOL "" FORCE)
    set(TRACY_STATIC ON CACHE BOOL "" FORCE)
    set(TRACY_CALLSTACK ON CACHE BOOL "" FORCE)
    
    if (MINGW)
        # Disable ETW / kernel system tracing
        set(TRACY_NO_SYSTEM_TRACING ON CACHE BOOL "" FORCE)
        #set(TRACY_NO_SAMPLING ON CACHE BOOL "" FORCE)
    endif()
    
    FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG master
        GIT_SHALLOW TRUE
        GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(tracy)
    set(DEP_INCLUDES ${DEP_INCLUDES} ${tracy_SOURCE_DIR})
        
endif ()