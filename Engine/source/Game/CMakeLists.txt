project(Game)
include(${CMAKE_SOURCE_DIR}/cmake/Library.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/CompileOptions.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Link.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Include.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Pch.cmake)

# Link projects
target_link_libraries(${PROJECT_NAME} PUBLIC
    Engine
    
    opencv_core
    opencv_imgproc
    opencv_video
    opencv_videoio
    opencv_highgui
)
