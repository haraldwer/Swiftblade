project(Content)
message("-- Creating library: " ${PROJECT_NAME})

# Copy default content
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION ${GAME_SRC})

# Glob from game dir
file(GLOB_RECURSE files
  "${GAME_CONTENT}/*.wgsl"
  "${GAME_CONTENT}/*.wgsli"
  "${GAME_CONTENT}/*.json"
  "${GAME_CONTENT}/*.txt"
  "${GAME_CONTENT}/*.ini"
  "${GAME_CONTENT}/*.png"
  "${GAME_CONTENT}/*.ttf"
  #"${CMAKE_CURRENT_SOURCE_DIR}/*.obj"
)

# Write dummy
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.c "")
add_library(${PROJECT_NAME} STATIC ${files} dummy.c)