project(Content)
message("-- Creating library: " ${PROJECT_NAME})

# Copy default content
file(COPY ${PROJECT_SOURCE_DIR}
    DESTINATION ${GAME_PATH})

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
file(WRITE ${GAME_CONTENT}/dummy.c "")
add_library(${PROJECT_NAME} STATIC ${files} ${GAME_CONTENT}/dummy.c)