file(GLOB_RECURSE libs "${CMAKE_SOURCE_DIR}/library/*.lib")
target_link_libraries(${PROJECT_NAME} ${libs})
