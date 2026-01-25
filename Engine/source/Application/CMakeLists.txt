project(Application)
include(${CMAKE_SOURCE_DIR}/cmake/Executable.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Link.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Include.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Pch.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/CompileOptions.cmake)

# Link projects
target_link_libraries(${PROJECT_NAME} PUBLIC
    Game
    Rendering
)

if (NOT RETAIL_MODE)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        Editor
    )
endif ()
