
if (UNIX)
	file(STRINGS ${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM_PATH}/static/libs.txt ordered_libs)
	foreach (lib ${ordered_libs})
		if (lib MATCHES "\.a$" OR lib MATCHES "\.so$")
			list(APPEND libs ${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM_PATH}/static/${lib})
		else ()
			list(APPEND libs ${lib})
		endif ()
	endforeach ()
	target_link_libraries(${PROJECT_NAME} ${libs})
endif (UNIX)

if (MSVC) # Uses config
	file(GLOB_RECURSE debug_libs 
		"${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM}_Debug/static/*.lib")
	file(GLOB_RECURSE release_libs
		"${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM}_Release/static/*.lib")
	target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Debug>:${debug_libs}>)
	target_link_libraries(${PROJECT_NAME} $<$<CONFIG:Release>:${release_libs}>)
endif (MSVC)
