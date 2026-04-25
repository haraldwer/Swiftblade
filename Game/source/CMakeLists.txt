project(Game)

# Set custom project path
set(PROJECT_SOURCE_DIR ${GAME_SRC})

include(${CMAKE_SOURCE_DIR}/cmake/Library.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/CompileOptions.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Link.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Include.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Pch.cmake)

# Link projects
target_link_libraries(${PROJECT_NAME} PUBLIC
    Engine
)
