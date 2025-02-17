message("-- Creating executable: " ${PROJECT_NAME})

message("-- Configuration: " ${CMAKE_GENERATOR_PLATFORM})

# Get sources
include(${CMAKE_SOURCE_DIR}/cmake/Source.cmake)

# Add exe
add_executable(${PROJECT_NAME} ${sources})

# Copy DLL files to exe folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/library/${CMAKE_GENERATOR_PLATFORM}_Debug/dynamic/>
    $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/library/${CMAKE_GENERATOR_PLATFORM}_Release/dynamic/>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)
