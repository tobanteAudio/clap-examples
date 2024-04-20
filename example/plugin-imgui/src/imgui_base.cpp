// platform independent connection between clap and imgui.
// plugin implementations should not need to touch this file.

#include "audio_plugin.hpp"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"
#include "imgui_internal.h"  // so we can get the viewport associated with an ImGui window
#include <GLFW/glfw3.h>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

// always in screen coordinates
void getNativeWindowPosition(void* display, void* window, int& x, int& y, int& w, int& h);
void setNativeParent(void* display, void* window, GLFWwindow* glfw_win);

auto createTimer(unsigned int ms) -> bool;
void destroyTimer();
auto getTickCount() -> unsigned int;

extern clap_host const* clapHost;

enum
{
    TIMER_MS = 30
};

GLFWwindow* gNativeWindow;
unsigned int gWantTeardown;
unsigned int gWindowCount;

struct GuiContext
{
    AudioPlugin* plugin;
    void* display;  // only used for x11
    void* window;
    char name[64];
    int did_parenting;
    GuiContext* next;
};

GuiContext* rec_list;

int render_pass_reentry;

void imguiDoRenderPass()
{
    if (gNativeWindow == nullptr) { return; }
    if (rec_list == nullptr) { return; }

    if (render_pass_reentry != 0) { return; }
    ++render_pass_reentry;  // glfwPollEvents can reenter us by pumping timer
                            // messages

    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    GuiContext* rec = rec_list;
    while (rec != nullptr) {
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;
        getNativeWindowPosition(rec->display, rec->window, x, y, w, h);
        ImGui::SetNextWindowPos(ImVec2(x, y));
        ImGui::SetNextWindowSize(ImVec2(w, h));

        static constexpr auto const imguiFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
                                               | ImGuiWindowFlags_NoDecoration
                                               | ImGuiWindowFlags_NoDocking;
        ImGui::Begin(rec->name, nullptr, imguiFlags);

        rec->plugin->draw();

        ImGui::End();

        rec = rec->next;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwSwapBuffers(gNativeWindow);

    rec = rec_list;
    while (rec != nullptr) {
        if (rec->did_parenting == 0) {
            ImGuiWindow* w = ImGui::FindWindowByName(rec->name);
            if ((w != nullptr) && (w->Viewport != nullptr) && w->Viewport->PlatformWindowCreated) {
                auto* glfw_win = (GLFWwindow*)w->Viewport->PlatformHandle;
                setNativeParent(rec->display, rec->window, glfw_win);
                rec->did_parenting = 1;
            }
        }
        rec = rec->next;
    }

    --render_pass_reentry;
}

void imguiTeardown()
{
    if (gNativeWindow == nullptr) { return; }

    destroyTimer();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(gNativeWindow);
    gNativeWindow = nullptr;
    glfwTerminate();
}

void imguiTimerCallback()
{
    if (gWantTeardown > 0) {
        if (getTickCount() > gWantTeardown) { imguiTeardown(); }
    } else {
        imguiDoRenderPass();
    }
}

static void glfw_error_callback(int error, char const* description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

auto imguiAttach(AudioPlugin* plugin, void* display, void* window) -> bool
{
    if ((plugin == nullptr) || (window == nullptr)) { return false; }
    if (plugin->uiContext != nullptr) { return true; }

    gWantTeardown = 0;
    if (gNativeWindow == nullptr) {
        if (!createTimer(TIMER_MS)) { return false; }

        glfwSetErrorCallback(glfw_error_callback);
        if (glfwInit() == 0) { return false; }

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        // invisible top level window
        gNativeWindow = glfwCreateWindow(1, 1, "ImGui Backend", nullptr, nullptr);
        if (gNativeWindow == nullptr) { return false; }

        glfwMakeContextCurrent(gNativeWindow);
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        ImGui::GetIO().ConfigViewportsNoAutoMerge     = true;
        ImGui::GetIO().ConfigViewportsNoTaskBarIcon   = true;
        ImGui::GetIO().ConfigViewportsNoDefaultParent = true;

        ImGui_ImplGlfw_InitForOpenGL(gNativeWindow, true);
        ImGui_ImplOpenGL3_Init(nullptr);
    }

    auto* new_rec = (GuiContext*)malloc(sizeof(GuiContext));
    memset(new_rec, 0, sizeof(GuiContext));
    new_rec->plugin  = plugin;
    new_rec->display = display;
    new_rec->window  = window;
    sprintf(new_rec->name, "%d:%p", ++gWindowCount, new_rec);
    plugin->uiContext = new_rec;

    if (rec_list != nullptr) { new_rec->next = rec_list; }
    rec_list = new_rec;

    return true;
}

auto AudioPlugin::destroyUI(bool is_plugin_destroy) -> bool
{
    if (uiContext != nullptr) {
        auto* old_rec = static_cast<GuiContext*>(uiContext);
        uiContext     = nullptr;

        GuiContext* prev_rec = nullptr;
        GuiContext* rec      = rec_list;
        while (rec != nullptr) {
            if (rec == old_rec) {
                if (prev_rec == nullptr) {
                    rec_list = old_rec->next;
                } else {
                    prev_rec->next = old_rec->next;
                }
                break;
            }
            prev_rec = rec;
            rec      = rec->next;
        }
        free(old_rec);
        // imgui should manage destroying the platform window once it's not used
        // in a render pass
    }

    if (rec_list == nullptr) {
        if (is_plugin_destroy) {
            imguiTeardown();
        } else {
            gWantTeardown = getTickCount() + 1000;
        }
    }

    return true;
}

void timerCallback(clap_plugin const* /*plugin*/, unsigned int /*timer_id*/) { imguiTimerCallback(); }

auto guiTimerSupport = clap_plugin_timer_support_t{timerCallback};

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
