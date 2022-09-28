// add generic support for extensions here.
// plugin implementations should not need to touch this file.

#pragma once

#include <clap/clap.h>

struct Plugin
{
    clap_plugin m_clap_plugin;
    clap_plugin_params m_clap_plugin_params;
    int m_w, m_h;

    clap_plugin_gui m_clap_plugin_gui;
    void* m_ui_ctx;

    Plugin(clap_plugin_descriptor const* descriptor, clap_host const* host);
    virtual ~Plugin();

    virtual bool init() = 0;
    virtual bool activate(double sr, uint32_t minFrames, uint32_t maxFrames)
        = 0;
    virtual void deactivate()                                        = 0;
    virtual bool startProcessing()                                   = 0;
    virtual void stopProcessing()                                    = 0;
    virtual clap_process_status process(clap_process const* process) = 0;
    virtual void const* getExtension(char const* id)                 = 0;
    virtual void onMainThread()                                      = 0;
    virtual void draw()                                              = 0;
    virtual bool getPreferredSize(uint32_t* width, uint32_t* height) = 0;

    bool isApiSupported(char const* api, bool is_floating);
    bool createUI(char const* api, bool is_floating);
    bool destroyUI(bool is_plugin_destroy);
    bool setScale(double scale);
    bool getSize(uint32_t* width, uint32_t* height);
    bool canResize();
    bool adjustSize(uint32_t* width, uint32_t* height);
    bool setSize(uint32_t width, uint32_t height);
    bool setParent(clap_window const* window);
    bool setTransient(clap_window const* window);
    void suggestTitle(char const* title);
    bool showUI();
    bool hideUI();

    virtual uint32_t numParameter()                                        = 0;
    virtual bool getParameterInfo(uint32_t index, clap_param_info_t* info) = 0;
    virtual bool getParameterValue(clap_id id, double* v)                  = 0;
    virtual bool valueToText(clap_id id, double v, char* display, uint32_t size)
        = 0;
    virtual bool textToValue(clap_id id, char const* display, double* v) = 0;
    virtual void
    flushParameter(clap_input_events const* in, clap_output_events const* out)
        = 0;
};

auto getGainPluginDescriptor() -> clap_plugin_descriptor*;
auto createGainPlugin(clap_host const* host) -> Plugin*;
