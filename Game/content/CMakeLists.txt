project(Content)
message("-- Creating library: " ${PROJECT_NAME})
file(GLOB_RECURSE files
  "${CMAKE_CURRENT_SOURCE_DIR}/*.vs"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.fs"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.ds"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.ps"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.si"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.json"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.txt"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.ini"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.ttf"
  #"${CMAKE_CURRENT_SOURCE_DIR}/*.obj"
)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.c "")
add_library(${PROJECT_NAME} STATIC ${files} dummy.c)