#pragma once

#include <functional>
#include <optional>
#include <string>

#include "canvas.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <cairo/cairo.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace mc {

enum struct ClickAction
{
    release = 0,
    press   = 1,
    repeat  = 2,
};

enum struct KeyModifier
{
    shift    = 0x01,
    control  = 0x02,
    alt      = 0x04,
    super    = 0x08,
    capsLock = 0x10,
    numLock  = 0x20,
};

enum struct MouseButton
{
    button1 = 0,
    button2 = 1,
    button3 = 2,
    button4 = 3,
    button5 = 4,
    button6 = 5,
    button7 = 6,
    button8 = 7,

    left   = button1,
    right  = button2,
    middle = button3,
};

enum struct MouseCursor
{
    arrow            = 0x00036001,
    ibeam            = 0x00036002,
    crosshair        = 0x00036003,
    hand             = 0x00036004,
    resizeHorizontal = 0x00036005,
    resizeVertical   = 0x00036006,
};

struct KeyEvent
{
    int keyCode;
    ClickAction action;
    KeyModifier modifier;
};

struct MouseEvent
{
    MouseButton button;
    ClickAction action;
    KeyModifier modifier;
};

template<typename T>
struct Point2D
{
    T x;
    T y;
};

struct Window
{
    Window(char const* name, int width, int height);
    ~Window();

    Window(Window const& other)                    = delete;
    auto operator=(Window const& other) -> Window& = delete;

    auto show() -> int;

    std::function<void(int, int)> sizeChanged{};
    std::function<void(int, int)> positionChanged{};

    std::function<void(KeyEvent)> keyClicked{};

    std::function<void()> mouseEnter{};
    std::function<void()> mouseExit{};
    std::function<void(MouseEvent)> mouseClicked{};
    std::function<void(Point2D<double>)> mouseMoved{};
    std::function<void(Point2D<double>)> mouseScrolled{};

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
