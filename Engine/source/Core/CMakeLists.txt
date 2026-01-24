project(Core)
include(${CMAKE_SOURCE_DIR}/cmake/Library.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Link.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Include.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Pch.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/CompileOptions.cmake)

# Link projects
target_link_libraries(${PROJECT_NAME} PUBLIC
    Utility
    tinyexpr-cpp
)

if (NOT RETAIL_MODE)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        ImGui
        TracyClient
    )
endif ()