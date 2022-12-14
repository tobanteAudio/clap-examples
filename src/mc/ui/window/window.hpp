#pragma once

#include <mc/core/functional.hpp>
#include <mc/core/memory.hpp>
#include <mc/ui/geometry/point_2d.hpp>
#include <mc/ui/geometry/rectangle.hpp>
#include <mc/ui/graphics/canvas.hpp>
#include <mc/ui/input/key_click_event.hpp>
#include <mc/ui/input/mouse_click_event.hpp>

namespace mc {

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
    UniquePtr<detail::WindowPimpl> _impl;
};
}  // namespace mc
