cmake_minimum_required(VERSION 3.21)
project(mc-ui)

find_package(cairo REQUIRED)
find_package(glfw3 REQUIRED)
find_package(mc-core REQUIRED)

add_library(mc-ui "mc/ui.cpp" "mc/ui.hpp")
add_library(mc::ui ALIAS mc-ui)
target_include_directories(mc-ui PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(mc-ui PUBLIC glfw::glfw cairo::cairo mc-core::mc-core)
target_sources(mc-ui
    PRIVATE
        "mc/ui/geometry/point_2d.hpp"
        "mc/ui/geometry/rectangle.hpp"

        "mc/ui/graphics/canvas.cpp"
        "mc/ui/graphics/canvas.hpp"
        "mc/ui/graphics/color_rgba.hpp"

        "mc/ui/input/click_action.hpp"
        "mc/ui/input/key_click_event.hpp"
        "mc/ui/input/key_modifier.hpp"
        "mc/ui/input/mouse_button.hpp"
        "mc/ui/input/mouse_click_event.hpp"
        "mc/ui/input/mouse_cursor.hpp"

        "mc/ui/window/window.cpp"
        "mc/ui/window/window.hpp"
)
