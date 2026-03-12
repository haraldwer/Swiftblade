message("-- Creating executable: " ${PROJECT_NAME})
message("-- Configuration: " ${CMAKE_GENERATOR_PLATFORM})

# Get sources
include(${CMAKE_SOURCE_DIR}/cmake/Source.cmake)

# Add exe
add_executable(${PROJECT_NAME} ${sources})

# Copy DLL files to exe folder
if (MSVC)
    set(DYNAMIC_LIB_SRC
        $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM}_Debug/dynamic/>
        $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM}_Release/dynamic/>)
else()
    set(DYNAMIC_LIB_SRC
        ${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM_PATH}/dynamic/)
endif()
if (EXISTS "${DYNAMIC_LIBS_SRC}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${DYNAMIC_LIB_SRC}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()

if (EMSCRIPTEN)
    # Actually link nakama?
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PUBLIC --preload-file=${CMAKE_SOURCE_DIR}/content@.)
    
    if (CMAKE_HOST_WIN32)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ./run_web.bat $<TARGET_FILE_DIR:${PROJECT_NAME}>
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    else ()
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ./run_web.sh $<TARGET_FILE_DIR:${PROJECT_NAME}>
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif ()
endif ()
  

