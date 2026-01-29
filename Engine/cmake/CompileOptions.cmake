message("-- Setting compile options for: ${PROJECT_NAME}")

# Shared definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    LOG_CATEGORY="${PROJECT_NAME}"
    PROJECT_VERSION="${PROJECT_VERSION}"
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    compile_options
)