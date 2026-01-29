project(Rendering)
include(${CMAKE_SOURCE_DIR}/cmake/Library.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/CompileOptions.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Link.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Include.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Pch.cmake)

# Link projects
target_link_libraries(${PROJECT_NAME} PUBLIC
    Core
    glfw
    glfw3webgpu
    webgpu
    meshoptimizer
)