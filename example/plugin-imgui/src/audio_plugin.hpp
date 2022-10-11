#pragma once

#include <clap/clap.h>

struct AudioPlugin
{
    AudioPlugin(clap_plugin_descriptor const* descriptor, clap_host const* host);
    virtual ~AudioPlugin();

    virtual auto init() -> bool                                                      = 0;
    virtual auto activate(double sr, uint32_t minFrames, uint32_t maxFrames) -> bool = 0;
    virtual auto deactivate() -> void                                                = 0;
    virtual auto startProcessing() -> bool                                           = 0;
    virtual auto stopProcessing() -> void                                            = 0;
    virtual auto process(clap_process const* ctx) -> clap_process_status             = 0;
    virtual auto getExtension(char const* id) -> void const*                         = 0;
    virtual auto onMainThread() -> void                                              = 0;
    virtual auto draw() -> void                                                      = 0;
    virtual auto getPreferredSize(uint32_t* width, uint32_t* height) -> bool         = 0;

    virtual auto numParameter() -> uint32_t                                                      = 0;
    virtual auto getParameterInfo(uint32_t idx, clap_param_info_t* info) -> bool                 = 0;
    virtual auto getParameterValue(clap_id id, double* v) -> bool                                = 0;
    virtual auto valueToText(clap_id id, double v, char* display, uint32_t size) -> bool         = 0;
    virtual auto textToValue(clap_id id, char const* display, double* v) -> bool                 = 0;
    virtual auto flushParameter(clap_input_events const* i, clap_output_events const* o) -> void = 0;

    auto isApiSupported(char const* api, bool is_floating) -> bool;
    auto createUI(char const* api, bool is_floating) -> bool;
    auto destroyUI(bool is_plugin_destroy) -> bool;
    auto setScale(double scale) -> bool;
    auto getSize(uint32_t* width, uint32_t* height) -> bool;
    auto canResize() -> bool;
    auto adjustSize(uint32_t* width, uint32_t* height) -> bool;
    auto setSize(uint32_t width, uint32_t height) -> bool;
    auto setParent(clap_window const* window) -> bool;
    auto setTransient(clap_window const* window) -> bool;
    auto suggestTitle(char const* title) -> void;
    auto showUI() -> bool;
    auto hideUI() -> bool;

    clap_plugin clapPluginHandle;
    clap_plugin_params clapParameterHandle;

    clap_plugin_gui clapGuiHandle;
    void* uiContext;
    int windowWidth;
    int windowHeight;
};
