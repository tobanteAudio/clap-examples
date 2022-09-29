// add generic support for extensions here.
// plugin implementations should not need to touch this file.

#pragma once

#include "audio_plugin.hpp"

inline static char const* GainPluginFeatures[] = {"", NULL};

inline static auto GainPluginDescriptor = clap_plugin_descriptor_t{
    .clap_version = CLAP_VERSION,
    .id           = "com.tobanteAudio.clap-imgui",
    .name         = "CLAP ImGui",
    .vendor       = "tobanteAudio",
    .url          = "https://github.com/tobanteAudio/clap-examples",
    .manual_url   = "https://github.com/tobanteAudio/clap-examples",
    .support_url  = "https://github.com/tobanteAudio/clap-examples",
    .version      = "0.1.0",
    .description  = "CLAP + GLFW + ImGui",
    .features     = GainPluginFeatures,
};

struct GainPlugin final : AudioPlugin
{
    enum
    {
        PARAM_VOLUME,
        PARAM_PAN,
        NUM_PARAMS
    };

    explicit GainPlugin(clap_host const* host);
    ~GainPlugin() override;

    bool init() override;
    bool activate(double sr, uint32_t minFrames, uint32_t maxFrames) override;
    void deactivate() override;
    bool startProcessing() override;
    void stopProcessing() override;
    clap_process_status process(clap_process const* ctx) override;

    template<class T>
    clap_process_status _process(
        clap_process const* process,
        int num_channels,
        int start_frame,
        int end_frame,
        double* start_param_values,
        double* end_param_values,
        T** in,
        T** out
    );

    void const* getExtension(char const* id);
    void onMainThread();
    void draw();
    bool getPreferredSize(uint32_t* width, uint32_t* height);

    uint32_t numParameter();
    bool getParameterInfo(uint32_t index, clap_param_info_t* info);
    bool getParameterValue(clap_id param_id, double* value);
    bool valueToText(clap_id param_id, double value, char* display, uint32_t size);
    bool textToValue(clap_id param_id, char const* display, double* value);
    void flushParameter(clap_input_events const* in, clap_output_events const* out);

    int m_srate;
    double m_param_values[NUM_PARAMS];
    double m_last_param_values[NUM_PARAMS];
    double m_peak_in[2], m_peak_out[2];
};

template<class T>
auto GainPlugin::_process(
    clap_process const* process,
    int num_channels,
    int start_frame,
    int end_frame,
    double* start_param_values,
    double* end_param_values,
    T** in,
    T** out
) -> clap_process_status
{
    if (!in || !out) return CLAP_PROCESS_ERROR;

    for (int c = 0; c < num_channels; ++c) {
        T *cin = in[c], *cout = out[c];
        if (!cin || !cout) return CLAP_PROCESS_ERROR;

        double adj = start_param_values[PARAM_VOLUME];
        if ((c & 1) && start_param_values[PARAM_PAN] < 0.0) {
            adj *= 1.0 + start_param_values[PARAM_PAN];
        } else if (!(c & 1) && start_param_values[PARAM_PAN] > 0.0) {
            adj *= 1.0 - start_param_values[PARAM_PAN];
        }

        double end_adj = end_param_values[PARAM_VOLUME];
        if ((c & 1) && end_param_values[PARAM_PAN] < 0.0) {
            end_adj *= 1.0 + end_param_values[PARAM_PAN];
        } else if (!(c & 1) && end_param_values[PARAM_PAN] > 0.0) {
            end_adj *= 1.0 - end_param_values[PARAM_PAN];
        }

        double d_adj = 0.0;
        if (end_frame > start_frame) { d_adj = (end_adj - adj) / (double)(end_frame - start_frame); }

        for (int i = start_frame; i < end_frame; ++i) {
            cout[i] = cin[i] * adj;
            if (cin[i] > m_peak_in[c]) m_peak_in[c] = cin[i];
            if (cout[i] > m_peak_out[c]) m_peak_out[c] = cout[i];
            adj += d_adj;
        }
    }

    return CLAP_PROCESS_CONTINUE;
}

auto getGainPluginDescriptor() -> clap_plugin_descriptor*;
auto createGainPlugin(clap_host const* host) -> AudioPlugin*;
