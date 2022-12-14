cmake_minimum_required(VERSION 3.21)
project(app-glfw-imgui-emscripten)

find_package(imgui CONFIG)
find_package(mc-core REQUIRED)

add_executable(app-glfw-imgui-emscripten
    main.cpp

    ${CMAKE_BINARY_DIR}/bindings/imgui_impl_sdl.h
    ${CMAKE_BINARY_DIR}/bindings/imgui_impl_sdl.cpp
    ${CMAKE_BINARY_DIR}/bindings/imgui_impl_opengl3.h
    ${CMAKE_BINARY_DIR}/bindings/imgui_impl_opengl3.cpp
)

set_target_properties(app-glfw-imgui-emscripten PROPERTIES SUFFIX ".html")
target_include_directories(app-glfw-imgui-emscripten PRIVATE ${CMAKE_BINARY_DIR})
target_compile_definitions(app-glfw-imgui-emscripten PRIVATE IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
target_link_libraries(app-glfw-imgui-emscripten PRIVATE imgui::imgui mc-core::mc-core)

target_compile_options(app-glfw-imgui-emscripten
    PRIVATE
        "SHELL:-s DISABLE_EXCEPTION_CATCHING=0"
        "SHELL:-s USE_BOOST_HEADERS=1"
        "SHELL:-s USE_SDL=2"
)
target_link_options(app-glfw-imgui-emscripten
    PRIVATE
        "SHELL:-s DISABLE_EXCEPTION_CATCHING=0"
        "SHELL:-s USE_BOOST_HEADERS=1"
        "SHELL:-s USE_SDL=2"
        "SHELL:-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1"
)
