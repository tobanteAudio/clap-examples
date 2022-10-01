cmake_minimum_required(VERSION 3.21)
project(app-cairo)

find_package(cairo CONFIG)
find_package(glfw3 CONFIG)

add_executable(app-cairo
    src/canvas.cpp
    src/canvas.hpp
    src/color.hpp
    src/main.cpp
)
target_link_libraries(app-cairo PRIVATE glfw::glfw cairo::cairo)
