
FetchContent_Declare(
    opencv
    GIT_REPOSITORY https://github.com/opencv/opencv.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

set(BUILD_LIST
    core
    imgproc
    imgcodecs
    features2d
    flann
    calib3d
    video
    videoio
    highgui
    CACHE STRING "" FORCE
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_WITH_STATIC_CRT OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_PERF_TESTS OFF CACHE BOOL "" FORCE)

set(BUILD_opencv_apps OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_java OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_python OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_js OFF CACHE BOOL "" FORCE)

set(WITH_OPENCL ON CACHE BOOL "" FORCE)
set(WITH_OPENGL ON CACHE BOOL "" FORCE)
set(WITH_OPENMP ON CACHE BOOL "" FORCE)
set(WITH_QT ON CACHE BOOL "" FORCE)
set(WITH_GSTREAMER ON CACHE BOOL "" FORCE)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND CMAKE_PREFIX_PATH
        "C:/Program Files/gstreamer/1.0/msvc_x86_64"
    )
endif()

set(WITH_IPP OFF CACHE BOOL "" FORCE)
set(WITH_TBB OFF CACHE BOOL "" FORCE)
set(WITH_CUDA OFF CACHE BOOL "" FORCE)
set(WITH_GTK OFF CACHE BOOL "" FORCE)
set(WITH_NVPL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(opencv)

list(APPEND DEP_INCLUDES 
    ${opencv_BINARY_DIR}
    ${OPENCV_MODULE_opencv_core_LOCATION}/include
    ${OPENCV_MODULE_opencv_imgproc_LOCATION}/include
    ${OPENCV_MODULE_opencv_imgcodecs_LOCATION}/include
    ${OPENCV_MODULE_opencv_flann_LOCATION}/include
    ${OPENCV_MODULE_opencv_features2d_LOCATION}/include
    ${OPENCV_MODULE_opencv_calib3d_LOCATION}/include
    ${OPENCV_MODULE_opencv_video_LOCATION}/include
    ${OPENCV_MODULE_opencv_videoio_LOCATION}/include
    ${OPENCV_MODULE_opencv_highgui_LOCATION}/include
)

include_directories(${CMAKE_BINARY_DIR})
