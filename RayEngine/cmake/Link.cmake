file(GLOB_RECURSE libs "${CMAKE_SOURCE_DIR}/library/${CMAKE_GENERATOR_PLATFORM}_${RUNTIME_CONF}/static/*.lib")
target_link_libraries(${PROJECT_NAME} ${libs})
