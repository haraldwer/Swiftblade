file(GLOB_RECURSE debug_libs 
	"${CMAKE_SOURCE_DIR}/library/${CMAKE_GENERATOR_PLATFORM}_Debug/static/*.lib")
file(GLOB_RECURSE release_libs
	"${CMAKE_SOURCE_DIR}/library/${CMAKE_GENERATOR_PLATFORM}_Release/static/*.lib")

target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:${debug_libs}>)
target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Release>:${release_libs}>)
