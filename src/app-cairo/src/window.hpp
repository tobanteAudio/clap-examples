#pragma once

#include <functional>
#include <optional>
#include <string>

#include "geometry/point_2d.hpp"
#include "graphics/canvas.hpp"
#include "input/key_click_event.hpp"
#include "input/mouse_click_event.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <cairo/cairo.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace mc {

struct Window
{
    Window(char const* name, int width, int height);
    ~Window();

    Window(Window const& other)                    = delete;
    auto operator=(Window const& other) -> Window& = delete;

    auto show() -> int;

    std::function<void(int, int)> sizeChanged{};
    std::function<void(int, int)> positionChanged{};

    std::function<void(KeyClickEvent)> keyClicked{};

    std::function<void()> mouseEnter{};
    std::function<void()> mouseExit{};
    std::function<void(MouseClickEvent)> mouseClicked{};
    std::function<void(Point2D<int>)> mouseMoved{};
    std::function<void(Point2D<int>)> mouseScrolled{};

    std::function<void(Canvas&)> draw{};

    auto updateCanvasSize(int w, int h) -> void;

private:
    GLFWwindow* _win{nullptr};
    cairo_surface_t* _surface;
    cairo_t* _ctx;
    std::optional<Canvas> _canvas;

    std::string _title;
    int _initialWidth;
    int _initialHeight;
};
}  // namespace mc
