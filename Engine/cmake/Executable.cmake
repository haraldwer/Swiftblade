message("-- Creating executable: " ${PROJECT_NAME})
message("-- Configuration: " ${CMAKE_GENERATOR_PLATFORM})

# Get sources
include(${CMAKE_SOURCE_DIR}/cmake/Source.cmake)

# Add exe
add_executable(${PROJECT_NAME} ${sources})

# Copy DLL files to exe folder

if (MSVC) # Uses config
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM}_Debug/dynamic/>
      $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM}_Release/dynamic/>
      $<TARGET_FILE_DIR:${PROJECT_NAME}>)
else()
  # TODO: Check if folder exists
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM_PATH}/dynamic/
      $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()


if (EMSCRIPTEN)

  # Actually link nakama?

  target_link_options(${PROJECT_NAME} PUBLIC --preload-file=${CMAKE_SOURCE_DIR}/content@.)
  
  # Glob all content files
#  file(GLOB_RECURSE content_files RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/content/*)
#  foreach (file ${content_files})
#    set(file_path ${file})
#    message("File: ${file_path}")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --preload-file ${file_path}")
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${file_path}")
#    target_link_options(${PROJECT_NAME} PUBLIC --preload-file=${file_path})
#  endforeach ()
  
  # Copy files to target
#  add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory 
#      ${CMAKE_SOURCE_DIR}/content 
#      $<TARGET_FILE_DIR:${PROJECT_NAME}>/content)
  
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ./run_web.sh $<TARGET_FILE_DIR:${PROJECT_NAME}>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif ()
  

