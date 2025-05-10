# Include what you use
option(ENABLE_IWYU "Enable include-what-you-use configuration (requires valid installation)" OFF)
if (ENABLE_IWYU)
  
  message("Include What You Use") 
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  
  find_package(Python3 COMPONENTS Interpreter)
  find_program(iwyu_tool_path NAMES iwyu_tool REQUIRED)
  if (iwyu_tool_path AND Python3_FOUND)
    add_custom_target(iwyu
            ALL      # Remove ALL if you don't iwyu to be run by default.
            COMMAND "${PYTHON_EXECUTABLE}" "${iwyu_tool_path}" -p "${CMAKE_BINARY_DIR}" --jobs 16
            COMMENT "Running include-what-you-use tool"
            VERBATIM
    )
  endif()
endif ()
