project(Content)
message("-- Creating library: " ${PROJECT_NAME})
file(GLOB_RECURSE files
  "${CMAKE_CURRENT_SOURCE_DIR}/*.vs"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.fs"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.json"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.txt"
)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.c "")
add_library(${PROJECT_NAME} STATIC ${files} dummy.c)