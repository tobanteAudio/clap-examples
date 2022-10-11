cmake_minimum_required(VERSION 3.21)
project(app-cairo)

add_executable(app-cairo main.cpp)
target_link_libraries(app-cairo PRIVATE mc::ui)
