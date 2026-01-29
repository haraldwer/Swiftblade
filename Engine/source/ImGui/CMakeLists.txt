
if (NOT RETAIL_MODE)
    
    project(ImGui)
    include(${CMAKE_SOURCE_DIR}/cmake/Library.cmake)
    
    target_include_directories(${PROJECT_NAME} PUBLIC 
        ${PROJECT_SOURCE_DIR}
    )

    # Link projects
    target_link_libraries(${PROJECT_NAME} PUBLIC
        glfw
        glfw3webgpu
        webgpu
    )
    
    if (${WEBGPU_BACKEND} STREQUAL WGPU)
        target_compile_definitions(${PROJECT_NAME} PRIVATE
            IMGUI_IMPL_WEBGPU_BACKEND_WGPU
        )
    else ()
        target_compile_definitions(${PROJECT_NAME} PRIVATE
            IMGUI_IMPL_WEBGPU_BACKEND_DAWN
        )
    endif ()

    if (UNIX AND GLFW_BUILD_X11)
        find_package(X11)
        if (X11_FOUND)
            target_link_libraries(${PROJECT_NAME} PUBLIC X11::X11)
        endif()
    endif ()
    
endif()