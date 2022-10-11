cmake_minimum_required(VERSION 3.21)
project(app-sdl)

find_package(cairo REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(app-sdl main.cpp)
target_include_directories(app-sdl PRIVATE ${CMAKE_BINARY_DIR})
target_compile_definitions(app-sdl PRIVATE IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
target_link_libraries(app-sdl PRIVATE cairo::cairo SDL2::SDL2)

# target_compile_options(app-sdl
#     PRIVATE
#         "SHELL:-s USE_SDL=2"
#         "SHELL:-s DISABLE_EXCEPTION_CATCHING=0"
# )
# target_link_options(app-sdl
#     PRIVATE
#         "SHELL:-s USE_SDL=2"
#         "SHELL:-s DISABLE_EXCEPTION_CATCHING=0"
#         "SHELL:-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1"
# )
