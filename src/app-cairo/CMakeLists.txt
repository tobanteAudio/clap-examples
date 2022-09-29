cmake_minimum_required(VERSION 3.21)
project(app-cairo)

find_package(cairo)

add_executable(app-cairo src/main.cpp)
target_link_libraries(app-cairo PRIVATE glfw::glfw cairo::cairo)
