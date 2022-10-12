// clang-format off
#include "imgui.h"
#include "bindings/imgui_impl_sdl.h"
#include "bindings/imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_opengles2.h>
// clang-format on

#include <mc/core/print.hpp>

#include <emscripten.h>

// Emscripten requires to have full control over the main loop. We're going to
// store our SDL book-keeping variables globally. Having a single function that
// acts as a loop prevents us to store state in the stack of said function. So
// we need some location for this.
SDL_Window* gWindow            = nullptr;
SDL_GLContext gGraphicsContext = nullptr;

// For clarity, our main loop code is declared at the end.
static auto mainLoop(void*) -> void;

int main(int, char**)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        mc::print("Error: {}\n", SDL_GetError());
        return -1;
    }

    // For the browser using Emscripten, we are going to use WebGL1 with GL ES2.
    // See the Makefile. for requirement details. It is very likely the
    // generated file won't work in many browsers. Firefox is the only sure bet,
    // but I have successfully run this code on Chrome for Android for example.
    char const* glsl_version = "#version 100";
    // const char* glsl_version = "#version 300 es";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

    gWindow = SDL_CreateWindow(
        "Title",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        (SDL_WindowFlags)flags
    );

    gGraphicsContext = SDL_GL_CreateContext(gWindow);
    if (!gGraphicsContext) {
        mc::print(stderr, "Failed to initialize WebGL context!\n");
        return 1;
    }
    SDL_GL_SetSwapInterval(1);  // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // For an Emscripten build we are disabling file-system access, so let's not
    // attempt to do a fopen() of the imgui.ini file. You may manually call
    // LoadIniSettingsFromMemory() to load settings from your own storage.
    ImGui::GetIO().IniFilename = nullptr;

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(gWindow, gGraphicsContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // This function call won't return, and will engage in an infinite loop,
    // processing events from the browser, and dispatching them.
    emscripten_set_main_loop_arg(mainLoop, nullptr, 0, true);
}

static auto mainLoop(void* arg) -> void
{
    // We can pass this argument as the second parameter of
    // emscripten_set_main_loop_arg(), but we don't use that.
    IM_UNUSED(arg);

    // Our state (make them static = more or less global) as a convenience to
    // keep the example terse.
    static bool show_demo_window = true;
    static ImVec4 color          = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    SDL_Event event;
    while (SDL_PollEvent(&event)) { ImGui_ImplSDL2_ProcessEvent(&event); }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in
    // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
    // ImGui!).
    if (show_demo_window) { ImGui::ShowDemoWindow(&show_demo_window); }

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair
    // to create a named window.
    {
        static float f     = 0.0f;
        static int counter = 0;

        // Create a window called "Hello, world!" and append into it.
        ImGui::Begin("Hello, world!");

        // Display some text (you can use a format strings too)
        ImGui::Text("This is some useful text.");

        // Edit bools storing our window open/close state
        ImGui::Checkbox("Demo Window", &show_demo_window);

        // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

        // Edit 3 floats representing a color
        ImGui::ColorEdit3("clear color", (float*)&color);

        // Buttons return true when clicked (most widgets return true
        // when edited/activated)
        if (ImGui::Button("Button")) { counter++; }
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        auto const fps = ImGui::GetIO().Framerate;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);
        ImGui::End();
    }

    // Rendering
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Render();
    SDL_GL_MakeCurrent(gWindow, gGraphicsContext);
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(color.x * color.w, color.y * color.w, color.z * color.w, color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(gWindow);
}
