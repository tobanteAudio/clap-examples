cmake_minimum_required(VERSION 3.21)
project(app-sdl2-imgui-emscripten)

add_executable(app-sdl2-imgui-emscripten
    main.cpp

    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_sdl2.h
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_sdl2.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.h
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.cpp
)

set_target_properties(app-sdl2-imgui-emscripten PROPERTIES SUFFIX ".html")
target_link_libraries(app-sdl2-imgui-emscripten PRIVATE ta::imgui)

target_compile_options(app-sdl2-imgui-emscripten
    PRIVATE
        "SHELL:-s DISABLE_EXCEPTION_CATCHING=0"
        "SHELL:-s USE_BOOST_HEADERS=1"
        "SHELL:-s USE_SDL=2"
)
target_link_options(app-sdl2-imgui-emscripten
    PRIVATE
        "SHELL:-s DISABLE_EXCEPTION_CATCHING=0"
        "SHELL:-s USE_BOOST_HEADERS=1"
        "SHELL:-s USE_SDL=2"
        "SHELL:-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1"
)
