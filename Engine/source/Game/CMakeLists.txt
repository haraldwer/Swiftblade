project(Game)
include(${CMAKE_SOURCE_DIR}/cmake/Executable.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Link.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Include.cmake)

target_include_directories(${PROJECT_NAME} 
  PRIVATE
    ${CMAKE_SOURCE_DIR}/source/Engine
)

include(${CMAKE_SOURCE_DIR}/cmake/Pch.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/CompileOptions.cmake)

# Link projects
target_link_libraries(${PROJECT_NAME} 
    Engine
)
