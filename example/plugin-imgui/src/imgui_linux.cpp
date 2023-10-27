#if defined(__linux__)

// clang-format off
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include "audio_plugin.hpp"

#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
// clang-format on

auto imguiAttach(AudioPlugin* plugin, void* native_display, void* native_window) -> bool;

auto AudioPlugin::isApiSupported(char const* api, bool is_floating) -> bool
{
    return (api != nullptr) && (strcmp(api, CLAP_WINDOW_API_X11) == 0) && !is_floating;
}

auto AudioPlugin::setParent(clap_window const* parentWindow) -> bool
{
    return (parentWindow != nullptr) && (parentWindow->x11 != 0U)
        && imguiAttach(this, XOpenDisplay(nullptr), (void*)parentWindow->x11);
}

void getNativeWindowPosition(
    void* native_display,
    void* native_window,
    int& x,
    int& y,
    int& w,
    int& h
)
{
    auto* xdisp     = (Display*)native_display;
    auto const xwin = (Window)native_window;

    XWindowAttributes xwa;
    XGetWindowAttributes(xdisp, xwin, &xwa);

    Window const xroot = DefaultRootWindow(xdisp);
    Window xchild      = 0;
    if (xroot != 0U) { XTranslateCoordinates(xdisp, xwin, xroot, 0, 0, &xwa.x, &xwa.y, &xchild); }

    x = xwa.x;
    y = xwa.y;
    w = xwa.width;
    h = xwa.height;
}

void setNativeParent(void* native_display, void* native_window, GLFWwindow* glfw_win)
{
    auto* xdisp     = (Display*)native_display;
    auto const xpar = (Window)native_window;
    auto const xwin = (Window)glfwGetX11Window(glfw_win);
    XReparentWindow(xdisp, xwin, xpar, 0, 0);
}

extern clap_host* clapHost;
unsigned int timer_id;

auto createTimer(unsigned int ms) -> bool
{
    auto* timer_support
        = (clap_host_timer_support*)clapHost->get_extension(clapHost, CLAP_EXT_TIMER_SUPPORT);
    return (timer_support != nullptr) && timer_support->register_timer(clapHost, ms, &timer_id);
}

void destroyTimer()
{
    auto* timer_support
        = (clap_host_timer_support*)clapHost->get_extension(clapHost, CLAP_EXT_TIMER_SUPPORT);
    if (timer_support != nullptr) { timer_support->unregister_timer(clapHost, timer_id); }
    timer_id = 0;
}

auto getTickCount() -> unsigned int
{
    struct timeval tm = {0};
    gettimeofday(&tm, nullptr);
    return (unsigned int)(tm.tv_sec * 1000 + tm.tv_usec / 1000);
}

#endif
