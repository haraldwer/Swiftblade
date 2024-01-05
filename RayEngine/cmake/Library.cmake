message("-- Creating library: " ${PROJECT_NAME})
include(${CMAKE_SOURCE_DIR}/cmake/Source.cmake)
add_library(${PROJECT_NAME} STATIC ${sources})