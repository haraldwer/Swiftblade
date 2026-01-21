message("-- Globbing sources: " ${PROJECT_NAME})

file(GLOB_RECURSE sources
  "${PROJECT_SOURCE_DIR}/*.h"
  "${PROJECT_SOURCE_DIR}/*.c"
  "${PROJECT_SOURCE_DIR}/*.cpp"
  "${PROJECT_SOURCE_DIR}/*.hpp"
)