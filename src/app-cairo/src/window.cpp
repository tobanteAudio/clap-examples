#include "window.hpp"

#include <cstdio>
#include <cstdlib>

#include <cairo/cairo-win32.h>

namespace mc {
static auto errorCallback(int error, char const* description) -> void
{
    std::fprintf(stderr, "Error: %d %s\n", error, description);
}

static auto useWindowUserPtr(GLFWwindow* backend, auto callback) -> void
{
    auto* ptr = glfwGetWindowUserPointer(backend);
    if (ptr == nullptr) { throw "No user pointer"; }
    callback(*static_cast<Window*>(ptr));
}

static auto keyCallback(GLFWwindow* backend, int key, int scancode, int action, int mods) -> void
{
    useWindowUserPtr(backend, [=](Window& win) {
        if (not win.keyClicked) { return; }
        auto state    = static_cast<ClickAction>(action);
        auto modifier = static_cast<KeyModifier>(mods);
        win.keyClicked(KeyClickEvent{key, state, modifier});
    });
}

static auto mouseButtonCallback(GLFWwindow* backend, int button, int action, int mods) -> void
{
    useWindowUserPtr(backend, [=](Window& win) {
        if (not win.mouseClicked) { return; }
        auto btn      = static_cast<MouseButton>(button);
        auto state    = static_cast<ClickAction>(action);
        auto modifier = static_cast<KeyModifier>(mods);
        win.mouseClicked(MouseClickEvent{btn, state, modifier});
    });
}

static auto mouseScrolledCallback(GLFWwindow* backend, double xoffset, double yoffset) -> void
{
    useWindowUserPtr(backend, [=](Window& win) {
        if (win.mouseScrolled) {
            win.mouseScrolled({static_cast<int>(xoffset), static_cast<int>(yoffset)});
        }
    });
}

static auto mouseMovedCallback(GLFWwindow* backend, double xpos, double ypos) -> void
{
    useWindowUserPtr(backend, [=](Window& win) {
        if (win.mouseMoved) { win.mouseMoved({static_cast<int>(xpos), static_cast<int>(ypos)}); }
    });
}

static auto mouseEnterCallback(GLFWwindow* backend, int entered) -> void
{
    useWindowUserPtr(backend, [=](Window& win) {
        if (entered == GLFW_TRUE) {
            if (win.mouseEnter) { win.mouseEnter(); }
            return;
        }

        if (win.mouseExit) { win.mouseExit(); }
    });
}

static auto windowSizeCallback(GLFWwindow* backend, int width, int height) -> void
{
    useWindowUserPtr(backend, [=](Window& win) {
        if (win.sizeChanged) { win.sizeChanged(width, height); }
    });
}

static auto windowPosCallback(GLFWwindow* backend, int width, int height) -> void
{
    useWindowUserPtr(backend, [=](Window& win) {
        if (win.positionChanged) { win.positionChanged(width, height); }
    });
}

static auto frameBufferResizedCallback(GLFWwindow* backend, int width, int height) -> void
{
    useWindowUserPtr(backend, [=](Window& win) { win.updateCanvasSize(width, height); });
}

Window::Window(char const* name, int width, int height)
    : _title{name}
    , _initialWidth{width}
    , _initialHeight{height}
{
    glfwSetErrorCallback(errorCallback);
    if (not glfwInit()) { throw EXIT_FAILURE; }
}

Window::~Window()
{
    if (_win) { glfwDestroyWindow(_win); }
    if (_ctx) { cairo_destroy(_ctx); }
    if (_surface) {
        cairo_surface_finish(_surface);
        cairo_surface_destroy(_surface);
    }
    glfwTerminate();
}

auto Window::show() -> int
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    _win = glfwCreateWindow(_initialWidth, _initialHeight, _title.c_str(), NULL, NULL);
    if (!_win) { return EXIT_FAILURE; }

    glfwSetWindowUserPointer(_win, this);
    glfwSetCursorEnterCallback(_win, mouseEnterCallback);
    glfwSetCursorPosCallback(_win, mouseMovedCallback);
    glfwSetFramebufferSizeCallback(_win, frameBufferResizedCallback);

    glfwSetKeyCallback(_win, keyCallback);
    glfwSetMouseButtonCallback(_win, mouseButtonCallback);
    glfwSetScrollCallback(_win, mouseScrolledCallback);
    glfwSetWindowPosCallback(_win, windowPosCallback);
    glfwSetWindowSizeCallback(_win, windowSizeCallback);

    // glfwSwapInterval(1);

    updateCanvasSize(_initialWidth, _initialHeight);

    while (not glfwWindowShouldClose(_win)) {
        if (draw) { draw(*_canvas); }

        cairo_paint(_ctx);
        cairo_surface_flush(_surface);

        // glfwSwapBuffers(_win);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}

auto Window::updateCanvasSize(int, int) -> void
{
    auto dc  = GetDC(glfwGetWin32Window(_win));
    _surface = cairo_win32_surface_create(dc);
    _ctx     = cairo_create(_surface);
    _canvas  = Canvas{_ctx};
}

}  // namespace mc
