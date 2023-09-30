#pragma once

#include <ta/ui/geometry/point_2d.hpp>
#include <ta/ui/geometry/rectangle.hpp>
#include <ta/ui/graphics/canvas.hpp>
#include <ta/ui/input/key_click_event.hpp>
#include <ta/ui/input/mouse_click_event.hpp>

#include <functional>
#include <memory>

namespace ta {

namespace detail {
struct WindowPimpl;
}

struct Window
{
    Window(char const* name, int width, int height);
    ~Window();

    auto show() -> int;

    std::function<void(int, int)> sizeChanged;
    std::function<void(Point2D<int>)> positionChanged;
    std::function<void(KeyClickEvent)> keyClicked;
    std::function<void()> mouseEnter;
    std::function<void()> mouseExit;
    std::function<void(MouseClickEvent)> mouseClicked;
    std::function<void(Point2D<int>)> mouseMoved;
    std::function<void(Point2D<int>)> mouseScrolled;
    std::function<void(Canvas&)> draw;

private:
    std::unique_ptr<detail::WindowPimpl> _impl;
};
}  // namespace ta
