cmake_minimum_required(VERSION 3.21)
project(ta-ui)

find_package(cairo REQUIRED)
find_package(glfw3 REQUIRED)

add_library(ta-ui "ta/ui.cpp" "ta/ui.hpp")
add_library(ta::ui ALIAS ta-ui)
target_include_directories(ta-ui PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(ta-ui PUBLIC glfw::glfw cairo::cairo)
target_sources(ta-ui
    PRIVATE
        "ta/ui/geometry/point_2d.hpp"
        "ta/ui/geometry/rectangle.hpp"

        "ta/ui/graphics/canvas.cpp"
        "ta/ui/graphics/canvas.hpp"
        "ta/ui/graphics/color_rgba.hpp"

        "ta/ui/input/click_action.hpp"
        "ta/ui/input/key_click_event.hpp"
        "ta/ui/input/key_modifier.hpp"
        "ta/ui/input/mouse_button.hpp"
        "ta/ui/input/mouse_click_event.hpp"
        "ta/ui/input/mouse_cursor.hpp"

        "ta/ui/window/window.cpp"
        "ta/ui/window/window.hpp"
)
