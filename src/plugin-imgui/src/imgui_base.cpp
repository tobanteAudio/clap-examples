// platform independent connection between clap and imgui.
// plugin implementations should not need to touch this file.

#include "audio_plugin.hpp"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"
#include "imgui_internal.h"  // so we can get the viewport associated with an ImGui window
#include <GLFW/glfw3.h>

// always in screen coordinates
void getNativeWindowPosition(void* display, void* window, int& x, int& y, int& w, int& h);
void setNativeParent(void* display, void* window, GLFWwindow* glfw_win);

bool createTimer(unsigned int ms);
void destroyTimer();
unsigned int getTickCount();

// extern clap_host const* clapHost;

#define TIMER_MS 30
GLFWwindow* backend_wnd;
unsigned int want_teardown;
unsigned int wnd_counter;

struct ui_ctx_rec
{
    AudioPlugin* plugin;
    void* display;  // only used for x11
    void* window;
    char name[64];
    int did_parenting;
    ui_ctx_rec* next;
};

ui_ctx_rec* rec_list;

int render_pass_reentry;

void imguiDoRenderPass()
{
    if (!backend_wnd) return;
    if (!rec_list) return;

    if (render_pass_reentry) return;
    ++render_pass_reentry;  // glfwPollEvents can reenter us by pumping timer
                            // messages

    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ui_ctx_rec* rec = rec_list;
    while (rec) {
        int x, y, w, h;
        getNativeWindowPosition(rec->display, rec->window, x, y, w, h);
        ImGui::SetNextWindowPos(ImVec2(x, y));
        ImGui::SetNextWindowSize(ImVec2(w, h));

        ImGui::Begin(
            rec->name,
            NULL,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration
                | ImGuiWindowFlags_NoDocking
        );

        rec->plugin->draw();

        ImGui::End();

        rec = rec->next;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwSwapBuffers(backend_wnd);

    rec = rec_list;
    while (rec) {
        if (!rec->did_parenting) {
            ImGuiWindow* w = ImGui::FindWindowByName(rec->name);
            if (w && w->Viewport && w->Viewport->PlatformWindowCreated) {
                GLFWwindow* glfw_win = (GLFWwindow*)w->Viewport->PlatformHandle;
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
    if (!backend_wnd) return;

    destroyTimer();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(backend_wnd);
    backend_wnd = NULL;
    glfwTerminate();
}

void imguiTimerCallback()
{
    if (want_teardown > 0) {
        if (getTickCount() > want_teardown) imguiTeardown();
    } else {
        imguiDoRenderPass();
    }
}

static void glfw_error_callback(int error, char const* description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

bool imguiAttach(AudioPlugin* plugin, void* display, void* window)
{
    if (!plugin || !window) { return false; }
    if (plugin->m_ui_ctx) { return true; }

    want_teardown = 0;
    if (!backend_wnd) {
        if (!createTimer(TIMER_MS)) { return false; }

        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) { return false; }

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        // invisible top level window
        backend_wnd = glfwCreateWindow(1, 1, "ImGui Backend", NULL, NULL);
        if (!backend_wnd) { return false; }

        glfwMakeContextCurrent(backend_wnd);
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        ImGui::GetIO().ConfigViewportsNoAutoMerge     = true;
        ImGui::GetIO().ConfigViewportsNoTaskBarIcon   = true;
        ImGui::GetIO().ConfigViewportsNoDefaultParent = true;

        ImGui_ImplGlfw_InitForOpenGL(backend_wnd, true);
        ImGui_ImplOpenGL3_Init(NULL);
    }

    ui_ctx_rec* new_rec = (ui_ctx_rec*)malloc(sizeof(ui_ctx_rec));
    memset(new_rec, 0, sizeof(ui_ctx_rec));
    new_rec->plugin  = plugin;
    new_rec->display = display;
    new_rec->window  = window;
    sprintf(new_rec->name, "%d:%p", ++wnd_counter, new_rec);
    plugin->m_ui_ctx = new_rec;

    if (rec_list) new_rec->next = rec_list;
    rec_list = new_rec;

    return true;
}

bool AudioPlugin::destroyUI(bool is_plugin_destroy)
{
    if (m_ui_ctx) {
        ui_ctx_rec* old_rec = (ui_ctx_rec*)m_ui_ctx;
        m_ui_ctx            = NULL;

        ui_ctx_rec *prev_rec = NULL, *rec = rec_list;
        while (rec) {
            if (rec == old_rec) {
                if (!prev_rec)
                    rec_list = old_rec->next;
                else
                    prev_rec->next = old_rec->next;
                break;
            }
            prev_rec = rec;
            rec      = rec->next;
        }
        free(old_rec);
        // imgui should manage destroying the platform window once it's not used
        // in a render pass
    }

    if (!rec_list) {
        if (is_plugin_destroy)
            imguiTeardown();
        else
            want_teardown = getTickCount() + 1000;
    }

    return true;
}

void timerCallback(clap_plugin const* plugin, unsigned int timer_id) { imguiTimerCallback(); }

auto guiTimerSupport = clap_plugin_timer_support_t{timerCallback};
