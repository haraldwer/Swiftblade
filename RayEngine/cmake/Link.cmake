
if (UNIX)
	file(STRINGS ${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM_PATH}/static/libs.txt ordered_libs)
	message("Ordered: ${ordered_libs}")
	foreach (lib ${ordered_libs})
		if (lib MATCHES "\.a$")
			list(APPEND libs ${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM_PATH}/static/${lib})
		else ()
			list(APPEND libs ${lib})
		endif ()
	endforeach ()
endif (UNIX)

if (MSVC)
	file(GLOB_RECURSE libs "${CMAKE_SOURCE_DIR}/library/${COMPILE_PLATFORM_PATH}/static/*.lib")
endif (MSVC)

message("Link: ${libs}")
target_link_libraries(${PROJECT_NAME} ${libs})
