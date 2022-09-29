// clang-format off

#include <cmath>
#include <cstdio>
#include <cstdint>

#include <cairo/cairo.h>
#include <cairo/cairo-win32.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// clang-format on

#include "canvas.hpp"

int main(int argc, char* argv[])
{
    using namespace mc;

    if (!glfwInit()) { return -1; }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto* window = glfwCreateWindow(800, 600, "Test", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSwapInterval(1);

    auto dc       = GetDC(glfwGetWin32Window(window));
    auto* surface = cairo_win32_surface_create(dc);
    auto* ctx     = cairo_create(surface);
    auto canvas   = Canvas{ctx};

    while (not glfwWindowShouldClose(window)) {
        {
            auto state = Canvas::ScopedSavedState{canvas};
            canvas.fillAll(Colors::gray);
        }

        cairo_paint(ctx);
        cairo_surface_flush(surface);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cairo_destroy(ctx);
    cairo_surface_finish(surface);
    cairo_surface_destroy(surface);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
