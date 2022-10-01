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

bool imguiAttach(AudioPlugin* plugin, void* native_display, void* native_window);

bool AudioPlugin::isApiSupported(char const* api, bool is_floating)
{
    return api && !strcmp(api, CLAP_WINDOW_API_X11) && !is_floating;
}

bool AudioPlugin::setParent(clap_window const* parentWindow)
{
    return parentWindow && parentWindow->x11
        && imguiAttach(this, XOpenDisplay(NULL), (void*)parentWindow->x11);
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
    Display* xdisp = (Display*)native_display;
    Window xwin    = (Window)native_window;

    XWindowAttributes xwa;
    XGetWindowAttributes(xdisp, xwin, &xwa);

    Window xroot = DefaultRootWindow(xdisp), xchild = 0;
    if (xroot) XTranslateCoordinates(xdisp, xwin, xroot, 0, 0, &xwa.x, &xwa.y, &xchild);

    x = xwa.x;
    y = xwa.y;
    w = xwa.width;
    h = xwa.height;
}

void setNativeParent(void* native_display, void* native_window, GLFWwindow* glfw_win)
{
    Display* xdisp = (Display*)native_display;
    Window xpar    = (Window)native_window;
    Window xwin    = (Window)glfwGetX11Window(glfw_win);
    XReparentWindow(xdisp, xwin, xpar, 0, 0);
}

extern clap_host* g_clap_host;
unsigned int timer_id;

bool createTimer(unsigned int ms)
{
    clap_host_timer_support* timer_support
        = (clap_host_timer_support*)g_clap_host->get_extension(g_clap_host, CLAP_EXT_TIMER_SUPPORT);
    return timer_support && timer_support->register_timer(g_clap_host, ms, &timer_id);
}

void destroyTimer()
{
    clap_host_timer_support* timer_support
        = (clap_host_timer_support*)g_clap_host->get_extension(g_clap_host, CLAP_EXT_TIMER_SUPPORT);
    if (timer_support) timer_support->unregister_timer(g_clap_host, timer_id);
    timer_id = 0;
}

unsigned int getTickCount()
{
    struct timeval tm = {0};
    gettimeofday(&tm, NULL);
    return (unsigned int)(tm.tv_sec * 1000 + tm.tv_usec / 1000);
}

#endif
