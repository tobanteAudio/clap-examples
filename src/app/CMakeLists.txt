cmake_minimum_required(VERSION 3.21)
project(app-glfw-imgui)

add_executable(app-glfw-imgui
    main.cpp
    opengl_shader.cpp
    file_manager.cpp
    opengl_shader.h
    file_manager.h

    ${CMAKE_BINARY_DIR}/bindings/imgui_impl_glfw.cpp
    ${CMAKE_BINARY_DIR}/bindings/imgui_impl_opengl3.cpp
    ${CMAKE_BINARY_DIR}/bindings/imgui_impl_glfw.h
    ${CMAKE_BINARY_DIR}/bindings/imgui_impl_opengl3.h

    assets/simple-shader.vs
    assets/simple-shader.fs
)

add_custom_command(TARGET app-glfw-imgui
    POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/simple-shader.vs $<TARGET_FILE_DIR:app-glfw-imgui>
    COMMAND
        ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/assets/simple-shader.fs $<TARGET_FILE_DIR:app-glfw-imgui>
)

target_include_directories(app-glfw-imgui PRIVATE ${CMAKE_BINARY_DIR})
target_compile_definitions(app-glfw-imgui PRIVATE IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
target_link_libraries(app-glfw-imgui PRIVATE imgui::imgui GLEW::glew_s glfw::glfw clap-core)