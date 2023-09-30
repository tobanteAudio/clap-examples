// clang-format off
#include "imgui.h"
#include "bindings/imgui_impl_sdl.h"
#include "bindings/imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_opengles2.h>
// clang-format on

#include <emscripten.h>

#include <cstdio>

struct Application
{
    SDL_Window* window            = nullptr;
    SDL_GLContext graphicsContext = nullptr;

    bool showDemoWindow = true;
    ImVec4 color        = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    int counter         = 0;
};

// For clarity, our main loop code is declared at the end.
static auto mainLoop(void*) -> void;

int main(int, char**)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        std::fprintf(stderr, "Error: {}\n", SDL_GetError());
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

    auto app   = Application{};
    app.window = SDL_CreateWindow(
        "Title",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        (SDL_WindowFlags)flags
    );

    app.graphicsContext = SDL_GL_CreateContext(app.window);
    if (!app.graphicsContext) {
        std::fprintf(stderr, "Failed to initialize WebGL context!\n");
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

    ImGui_ImplSDL2_InitForOpenGL(app.window, app.graphicsContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    emscripten_set_main_loop_arg(mainLoop, static_cast<void*>(&app), 0, true);
}

static auto mainLoop(void* arg) -> void
{
    auto* app   = static_cast<Application*>(arg);
    auto& color = app->color;

    auto event = SDL_Event{};
    while (SDL_PollEvent(&event)) { ImGui_ImplSDL2_ProcessEvent(&event); }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if (app->showDemoWindow) { ImGui::ShowDemoWindow(&app->showDemoWindow); }

    ImGui::Begin("Hello, world!");
    ImGui::Text("This is some useful text.");
    ImGui::Checkbox("Demo Window", &app->showDemoWindow);
    ImGui::ColorEdit3("clear color", (float*)&color);

    if (ImGui::Button("Button")) { app->counter++; }
    ImGui::SameLine();
    ImGui::Text("counter = %d", app->counter);

    auto const fps = ImGui::GetIO().Framerate;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);
    ImGui::End();

    ImGuiIO& io = ImGui::GetIO();
    ImGui::Render();
    SDL_GL_MakeCurrent(app->window, app->graphicsContext);
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(color.x * color.w, color.y * color.w, color.z * color.w, color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(app->window);
}
