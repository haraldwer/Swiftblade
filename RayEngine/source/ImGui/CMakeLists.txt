project(ImGui)
include(${CMAKE_SOURCE_DIR}/cmake/Library.cmake)

set_target_properties(${PROJECT_NAME} 
    PROPERTIES CXX_STANDARD 20
)

# Link projects
target_link_libraries(${PROJECT_NAME} 
    raylib
)
