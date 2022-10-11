#pragma once

#include <functional>

#include "geometry/point_2d.hpp"
#include "graphics/canvas.hpp"
#include "input/key_click_event.hpp"
#include "input/mouse_click_event.hpp"

namespace mc {

namespace detail {
struct WindowPimpl;
}

struct Window
{
    Window(char const* name, int width, int height);
    ~Window();

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

private:
    std::unique_ptr<detail::WindowPimpl> _impl;
};
}  // namespace mc
