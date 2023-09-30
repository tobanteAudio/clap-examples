#include "window.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <cairo/cairo-win32.h>
#include <cairo/cairo.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <cstdio>
#include <cstdlib>
#include <optional>
#include <string>

namespace ta {

namespace detail {
struct WindowPimpl
{
    WindowPimpl(Window& win, char const* name, int width, int height);
    ~WindowPimpl();

    WindowPimpl(WindowPimpl const& other)                    = delete;
    auto operator=(WindowPimpl const& other) -> WindowPimpl& = delete;

    auto show() -> int;
    auto updateCanvasSize() -> void;

    Window& window;
    GLFWwindow* glfwWindow{nullptr};
    cairo_surface_t* surface;
    cairo_t* ctx;
    std::optional<Canvas> canvas;

    std::string title;
    int initialWidth;
    int initialHeight;
};

static auto errorCallback(int error, char const* description) -> void
{
    std::fprintf(stderr, "Error: %d %s\n", error, description);
}

static auto useWindowUserPtr(GLFWwindow* backend, auto callback) -> void
{
    auto* ptr = glfwGetWindowUserPointer(backend);
    if (ptr == nullptr) { throw "No user pointer"; }
    callback(*static_cast<WindowPimpl*>(ptr));
}

static auto keyCallback(GLFWwindow* backend, int key, int scancode, int action, int mods) -> void
{
    useWindowUserPtr(backend, [=](WindowPimpl& impl) {
        if (not impl.window.keyClicked) { return; }
        auto state    = static_cast<ClickAction>(action);
        auto modifier = static_cast<KeyModifier>(mods);
        impl.window.keyClicked(KeyClickEvent{key, state, modifier});
    });
}

static auto mouseButtonCallback(GLFWwindow* backend, int button, int action, int mods) -> void
{
    useWindowUserPtr(backend, [=](WindowPimpl& impl) {
        if (not impl.window.mouseClicked) { return; }
        auto btn      = static_cast<MouseButton>(button);
        auto state    = static_cast<ClickAction>(action);
        auto modifier = static_cast<KeyModifier>(mods);
        impl.window.mouseClicked(MouseClickEvent{btn, state, modifier});
    });
}

static auto mouseScrolledCallback(GLFWwindow* backend, double xoffset, double yoffset) -> void
{
    useWindowUserPtr(backend, [=](WindowPimpl& impl) {
        if (impl.window.mouseScrolled) {
            impl.window.mouseScrolled({static_cast<int>(xoffset), static_cast<int>(yoffset)});
        }
    });
}

static auto mouseMovedCallback(GLFWwindow* backend, double xpos, double ypos) -> void
{
    useWindowUserPtr(backend, [=](WindowPimpl& impl) {
        if (impl.window.mouseMoved) {
            impl.window.mouseMoved({static_cast<int>(xpos), static_cast<int>(ypos)});
        }
    });
}

static auto mouseEnterCallback(GLFWwindow* backend, int entered) -> void
{
    useWindowUserPtr(backend, [=](WindowPimpl& impl) {
        if (entered == GLFW_TRUE) {
            if (impl.window.mouseEnter) { impl.window.mouseEnter(); }
            return;
        }

        if (impl.window.mouseExit) { impl.window.mouseExit(); }
    });
}

static auto windowSizeCallback(GLFWwindow* backend, int width, int height) -> void
{
    useWindowUserPtr(backend, [=](WindowPimpl& impl) {
        if (impl.window.sizeChanged) { impl.window.sizeChanged(width, height); }
    });
}

static auto windowPosCallback(GLFWwindow* backend, int width, int height) -> void
{
    useWindowUserPtr(backend, [=](WindowPimpl& impl) {
        if (impl.window.positionChanged) { impl.window.positionChanged({width, height}); }
    });
}

static auto frameBufferResizedCallback(GLFWwindow* backend, int width, int height) -> void
{
    useWindowUserPtr(backend, [=](WindowPimpl& impl) { impl.updateCanvasSize(); });
}

WindowPimpl::WindowPimpl(Window& win, char const* name, int width, int height)
    : window{win}
    , title{name}
    , initialWidth{width}
    , initialHeight{height}
{
    glfwSetErrorCallback(errorCallback);
    if (not glfwInit()) { throw EXIT_FAILURE; }
}

WindowPimpl::~WindowPimpl()
{
    if (glfwWindow) { glfwDestroyWindow(glfwWindow); }
    if (ctx) { cairo_destroy(ctx); }
    if (surface) {
        cairo_surface_finish(surface);
        cairo_surface_destroy(surface);
    }
    glfwTerminate();
}

auto WindowPimpl::show() -> int
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindow = glfwCreateWindow(initialWidth, initialHeight, title.c_str(), nullptr, nullptr);
    if (!glfwWindow) { return EXIT_FAILURE; }

    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetCursorEnterCallback(glfwWindow, mouseEnterCallback);
    glfwSetCursorPosCallback(glfwWindow, mouseMovedCallback);
    glfwSetFramebufferSizeCallback(glfwWindow, frameBufferResizedCallback);
    glfwSetKeyCallback(glfwWindow, keyCallback);
    glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
    glfwSetScrollCallback(glfwWindow, mouseScrolledCallback);
    glfwSetWindowPosCallback(glfwWindow, windowPosCallback);
    glfwSetWindowSizeCallback(glfwWindow, windowSizeCallback);

    updateCanvasSize();

    while (not glfwWindowShouldClose(glfwWindow)) {
        if (window.draw) {
            auto savedState = Canvas::ScopedSavedState{*canvas};
            window.draw(*canvas);
        }

        cairo_paint(ctx);
        cairo_surface_flush(surface);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}

auto WindowPimpl::updateCanvasSize() -> void
{
    auto dc = GetDC(glfwGetWin32Window(glfwWindow));
    surface = cairo_win32_surface_create(dc);
    ctx     = cairo_create(surface);
    canvas  = Canvas{ctx};
}

}  // namespace detail

Window::Window(char const* name, int width, int height)
    : _impl{std::make_unique<detail::WindowPimpl>(*this, name, width, height)}
{}

Window::~Window() = default;

auto Window::show() -> int { return _impl->show(); }

}  // namespace ta
