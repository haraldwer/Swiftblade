message("-- Creating executable: " ${PROJECT_NAME})

if(NOT RUNTIME_CONF)
  set(RUNTIME_CONF Debug)
endif()

message("-- Configuration: " ${CMAKE_GENERATOR_PLATFORM}_${RUNTIME_CONF})

# Get sources
include(${CMAKE_SOURCE_DIR}/cmake/Source.cmake)

# Add exe
add_executable(${PROJECT_NAME} ${sources})

# Compile options (00:01:44.927 -> 00:00:14.638)
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD true) # Unity builds
target_compile_options(${PROJECT_NAME} PUBLIC /MP) # Multithreading

# Copy DLL files to exe folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/library/${CMAKE_GENERATOR_PLATFORM}_${RUNTIME_CONF}/dynamic/ $<TARGET_FILE_DIR:${PROJECT_NAME}>)

unset(RUNTIME_CONF CACHE)