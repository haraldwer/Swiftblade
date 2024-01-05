project(ImGui)
include(${CMAKE_SOURCE_DIR}/cmake/Library.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/CompileOptions.cmake)

# Link projects
target_link_libraries(${PROJECT_NAME} 
    raylib
)
