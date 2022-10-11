cmake_minimum_required(VERSION 3.21)
project(app-cairo)

find_package(cairo CONFIG)
find_package(glfw3 CONFIG)

add_executable(app-cairo src/main.cpp)
target_sources(app-cairo
    PRIVATE
        src/canvas.cpp
        src/canvas.hpp
        src/color.hpp
        src/window.cpp
        src/window.hpp
)
target_link_libraries(app-cairo PRIVATE glfw::glfw cairo::cairo)
