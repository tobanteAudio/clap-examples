cmake_minimum_required(VERSION 3.21)
project(app-cairo)

find_package(cairo CONFIG)
find_package(glfw3 CONFIG)

add_executable(app-cairo src/main.cpp)
target_include_directories(app-cairo PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(app-cairo PRIVATE glfw::glfw cairo::cairo)
target_sources(app-cairo
    PRIVATE
        src/geometry/point_2d.hpp

        src/input/click_action.hpp
        src/input/key_click_event.hpp
        src/input/key_modifier.hpp
        src/input/mouse_button.hpp
        src/input/mouse_click_event.hpp
        src/input/mouse_cursor.hpp

        src/graphics/canvas.cpp
        src/graphics/canvas.hpp
        src/graphics/color.hpp

        src/window.cpp
        src/window.hpp
)