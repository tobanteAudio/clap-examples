// imgui helpers for win.
// plugin implementations should not need to touch this file.

#ifdef _WIN32

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include "main.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

bool imguiAttach(Plugin* plugin, void* display, void* window);

bool Plugin::isApiSupported(char const* api, bool is_floating)
{
    return api && !strcmp(api, CLAP_WINDOW_API_WIN32) && !is_floating;
}

bool Plugin::setParent(clap_window const* parent)
{
    return parent && parent->win32 && imguiAttach(this, NULL, parent->win32);
}

void getNativeWindowPosition(
    void* display,
    void* window,
    int& x,
    int& y,
    int& w,
    int& h
)
{
    RECT wr;
    ::GetWindowRect((HWND)window, &wr);
    x = wr.left;
    y = wr.top;
    w = wr.right - wr.left;
    h = wr.bottom - wr.top;
}

void setNativeParent(void* display, void* window, GLFWwindow* glfw_win)
{
    HWND hpar = (HWND)window;
    HWND hwnd = (HWND)glfwGetWin32Window(glfw_win);
    SetParent(hwnd, hpar);
    long style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_POPUP;
    style |= WS_CHILDWINDOW;
    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(
        hwnd,
        NULL,
        0,
        0,
        0,
        0,
        SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE
    );
}

unsigned int timer_id;

void CALLBACK timerCallback(HWND hwnd, UINT a, UINT_PTR b, DWORD c)
{
    extern void imguiTimerCallback();
    imguiTimerCallback();
}

bool createTimer(unsigned int ms)
{
    timer_id = SetTimer(NULL, 1, ms, timerCallback);
    return true;
}

void destroyTimer()
{
    KillTimer(NULL, timer_id);
    timer_id = 0;
}

unsigned int getTickCount() { return GetTickCount(); }

#endif