// clang-format off

#include <math.h>
#include <stdio.h>

#include <cairo/cairo.h>
#include <cairo/cairo-win32.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// clang-format on

int main(int argc, char* argv[])
{
    /* Initialize the library */
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto* window = glfwCreateWindow(800, 600, "Test", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSwapInterval(1);

    HDC dc                   = GetDC(glfwGetWin32Window(window));
    cairo_surface_t* surface = cairo_win32_surface_create(dc);
    cairo_t* ctx             = cairo_create(surface);

    while (!glfwWindowShouldClose(window)) {

        // have to group, else the x server does weird queueing
        // cairo_push_group(ctx);

        // clear
        cairo_set_source_rgb(ctx, 1.0, 1.0, 0.0);
        cairo_set_operator(ctx, CAIRO_OPERATOR_SOURCE);
        cairo_paint(ctx);

        // RENDER HERE

        // ungroup and actually display
        // cairo_pop_group_to_source(ctx);
        cairo_paint(ctx);
        cairo_surface_flush(surface);
        glfwSwapBuffers(window);

        glfwPollEvents();

        // glfwSetWindowShouldClose(window, 1);
    }

    cairo_destroy(ctx);
    cairo_surface_finish(surface);
    cairo_surface_destroy(surface);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
