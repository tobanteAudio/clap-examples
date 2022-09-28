// imgui helpers for win.
// plugin implementations should not need to touch this file.

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>

#include "main.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

bool imguiAttach(Plugin *plugin, void *native_display, void *native_window);

bool Plugin::isApiSupported(const char *api, bool is_floating) {
  return api && !strcmp(api, CLAP_WINDOW_API_WIN32) && !is_floating;
}

bool Plugin::setParent(const clap_window *parent) {
  return parent && parent->win32 && imguiAttach(this, NULL, parent->win32);
}

void getNativeWindowPosition(void *native_display, void *native_window, int *x,
                             int *y, int *w, int *h) {
  RECT wr;
  GetWindowRect((HWND)native_window, &wr);
  *x = wr.left;
  *y = wr.top;
  *w = wr.right - wr.left;
  *h = wr.bottom - wr.top;
}

void setNativeParent(void *native_display, void *native_window,
                     GLFWwindow *glfw_win) {
  HWND hpar = (HWND)native_window;
  HWND hwnd = (HWND)glfwGetWin32Window(glfw_win);
  SetParent(hwnd, hpar);
  long style = GetWindowLong(hwnd, GWL_STYLE);
  style &= ~WS_POPUP;
  style |= WS_CHILDWINDOW;
  SetWindowLong(hwnd, GWL_STYLE, style);
  SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
               SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

unsigned int timer_id;

void CALLBACK timer_proc(HWND hwnd, UINT a, UINT_PTR b, DWORD c) {
  extern void imguiOnTimer();
  imguiOnTimer();
}

bool createTimer(unsigned int ms) {
  timer_id = SetTimer(NULL, 1, ms, timer_proc);
  return true;
}

void destroyTimer() {
  KillTimer(NULL, timer_id);
  timer_id = 0;
}

unsigned int getTickCount() { return GetTickCount(); }