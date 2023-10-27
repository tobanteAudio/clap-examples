// add generic support for extensions here.
// plugin implementations should not need to touch this file.

#pragma once

#include <array>

#include "audio_plugin.hpp"

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

    auto init() -> bool override;
    auto activate(double sr, uint32_t minFrames, uint32_t maxFrames) -> bool override;
    auto deactivate() -> void override;
    auto startProcessing() -> bool override;
    auto stopProcessing() -> void override;
    auto process(clap_process const* ctx) -> clap_process_status override;

    auto numParameter() -> uint32_t override;
    auto getParameterInfo(uint32_t index, clap_param_info_t* info) -> bool override;
    auto getParameterValue(clap_id param_id, double* value) -> bool override;
    auto valueToText(clap_id param_id, double value, char* display, uint32_t size) -> bool override;
    auto textToValue(clap_id param_id, char const* display, double* value) -> bool override;
    auto flushParameter(clap_input_events const* in, clap_output_events const* out) -> void override;

    auto getExtension(char const* id) -> void const* override;
    auto onMainThread() -> void override;
    auto draw() -> void override;
    auto getPreferredSize(uint32_t* width, uint32_t* height) -> bool override;

private:
    template<class T>
    auto processInternal(
        clap_process const* ctx,
        int numChannels,
        int startFrame,
        int endFrame,
        double const* startParamValues,
        double const* endParamValues,
        T** in,
        T** out
    ) -> clap_process_status;

    double _sampleRate{48000.0};
    std::array<double, NUM_PARAMS> _parameterValues{};
    std::array<double, NUM_PARAMS> _lastParameterValues{};
    std::array<double, 2> _peakIn{};
    std::array<double, 2> _peakOut{};
};

template<class T>
auto GainPlugin::processInternal(
    clap_process const* /*ctx*/,
    int numChannels,
    int startFrame,
    int endFrame,
    double const* startParamValues,
    double const* endParamValues,
    T** in,
    T** out
) -> clap_process_status
{
    if (!in || !out) { return CLAP_PROCESS_ERROR; }

    for (int c = 0; c < numChannels; ++c) {
        T* cin  = in[c];
        T* cout = out[c];
        if (!cin || !cout) { return CLAP_PROCESS_ERROR; }

        double adj = startParamValues[PARAM_VOLUME];
        if ((c & 1) && startParamValues[PARAM_PAN] < 0.0) {
            adj *= 1.0 + startParamValues[PARAM_PAN];
        } else if (!(c & 1) && startParamValues[PARAM_PAN] > 0.0) {
            adj *= 1.0 - startParamValues[PARAM_PAN];
        }

        double end_adj = endParamValues[PARAM_VOLUME];
        if ((c & 1) && endParamValues[PARAM_PAN] < 0.0) {
            end_adj *= 1.0 + endParamValues[PARAM_PAN];
        } else if (!(c & 1) && endParamValues[PARAM_PAN] > 0.0) {
            end_adj *= 1.0 - endParamValues[PARAM_PAN];
        }

        double d_adj = 0.0;
        if (endFrame > startFrame) { d_adj = (end_adj - adj) / (double)(endFrame - startFrame); }

        for (int i = startFrame; i < endFrame; ++i) {
            cout[i] = cin[i] * adj;
            if (cin[i] > _peakIn[c]) { _peakIn[c] = cin[i]; }
            if (cout[i] > _peakOut[c]) { _peakOut[c] = cout[i]; }
            adj += d_adj;
        }
    }

    return CLAP_PROCESS_CONTINUE;
}

auto getGainPluginDescriptor() -> clap_plugin_descriptor*;
auto createGainPlugin(clap_host const* host) -> AudioPlugin*;
