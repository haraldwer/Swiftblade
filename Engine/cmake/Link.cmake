if (UNIX)
    file(STRINGS ${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM_PATH}/static/libs.txt ordered_libs)
    foreach (lib ${ordered_libs})
        if (lib MATCHES "\.a$" OR lib MATCHES "\.so$")
            list(APPEND libs ${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM_PATH}/static/${lib})
        else ()
            list(APPEND libs ${lib})
        endif ()
    endforeach ()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${libs})
endif ()

if (MSVC OR MINGW) # Uses config
    file(GLOB_RECURSE debug_libs
            "${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM}_Debug/static/*.lib")
    file(GLOB_RECURSE release_libs
            "${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM}_Release/static/*.lib")
    target_link_libraries(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:${debug_libs}>)
    target_link_libraries(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:${release_libs}>)
    
    if (MINGW)
        target_link_libraries(${PROJECT_NAME} PRIVATE iphlpapi)
    endif()
    
endif ()