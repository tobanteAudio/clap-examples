// plugin implementation.
// note this example does very little error checking,
// uses unsafe sprintf/strcpy, etc.

#include "gain_plugin.hpp"

#include "imgui.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static clap_param_info const param_info[GainPlugin::NUM_PARAMS] = {
    {0, CLAP_PARAM_REQUIRES_PROCESS, nullptr, "Volume", "",  -60.0,  12.0, 0.0},
    {1, CLAP_PARAM_REQUIRES_PROCESS, nullptr,    "Pan", "", -100.0, 100.0, 0.0}
};

static auto paramConvertValue(clap_id id, double in) -> double
{
    if (id < 0 || id >= GainPlugin::NUM_PARAMS) { return 0.0; }
    if (id == GainPlugin::PARAM_VOLUME) { return in > -150.0 ? pow(10.0, in / 20.0) : 0.0; }
    if (id == GainPlugin::PARAM_PAN) { return 0.01 * in; }
    return 0.0;
}

GainPlugin::GainPlugin(clap_host const* host) : AudioPlugin(getGainPluginDescriptor(), host)
{

    for (int i = 0; i < NUM_PARAMS; ++i) {
        _parameterValues[i] = _lastParameterValues[i]
            = paramConvertValue(i, param_info[i].default_value);
    }
    _peakIn[0] = _peakIn[1] = _peakOut[0] = _peakOut[1] = 0.0;
}

GainPlugin::~GainPlugin() = default;

auto GainPlugin::init() -> bool { return true; }

auto GainPlugin::activate(double sr, uint32_t /*minFrames*/, uint32_t /*maxFrames*/) -> bool
{
    _sampleRate = sr;
    _peakIn[0] = _peakIn[1] = _peakOut[0] = _peakOut[1] = 0.0;
    return true;
}

void GainPlugin::deactivate() {}

auto GainPlugin::startProcessing() -> bool { return true; }

void GainPlugin::stopProcessing() {}

auto GainPlugin::process(clap_process const* ctx) -> clap_process_status
{
    double const decay = pow(0.5, (double)ctx->frames_count / _sampleRate / 0.125);
    for (int c = 0; c < 2; ++c) {
        _peakIn[c] *= decay;
        _peakOut[c] *= decay;
        if (_peakIn[c] < 1.0e-6) { _peakIn[c] = 0.0; }
        if (_peakOut[c] < 1.0e-6) { _peakOut[c] = 0.0; }
    }

    double cur_param_values[NUM_PARAMS];
    for (int i = 0; i < NUM_PARAMS; ++i) { cur_param_values[i] = _parameterValues[i]; }

    clap_process_status s = -1;
    if ((ctx != nullptr) && ctx->audio_inputs_count == 1 && ctx->audio_inputs[0].channel_count == 2
        && ctx->audio_outputs_count == 1 && ctx->audio_outputs[0].channel_count == 2) {
        // handling incoming parameter changes and slicing the process call
        // on the time axis would happen here.

        if ((ctx->audio_inputs[0].data32 != nullptr) && (ctx->audio_outputs[0].data32 != nullptr)) {
            s = processInternal(
                ctx,
                2,
                0,
                ctx->frames_count,
                _lastParameterValues.data(),
                cur_param_values,
                ctx->audio_inputs[0].data32,
                ctx->audio_outputs[0].data32
            );
        } else if ((ctx->audio_inputs[0].data64 != nullptr) && (ctx->audio_outputs[0].data64 != nullptr)) {
            s = processInternal(
                ctx,
                2,
                0,
                ctx->frames_count,
                _lastParameterValues.data(),
                cur_param_values,
                ctx->audio_inputs[0].data64,
                ctx->audio_outputs[0].data64
            );
        }
    }

    for (int i = 0; i < NUM_PARAMS; ++i) { _lastParameterValues[i] = _parameterValues[i]; }

    if (s < 0) { s = CLAP_PROCESS_ERROR; }
    return s;
}

auto GainPlugin::getExtension(char const* /*id*/) -> void const* { return nullptr; }

void GainPlugin::onMainThread() {}

void GainPlugin::draw()
{
    ImGui::Text("Volume/Pan");

    for (int c = 0; c < 2; ++c) {
        double val = 0.0;
        if (_peakIn[c] > 0.001) {
            double const db = log(_peakIn[c]) * 20.0 / log(10.0);
            val             = (db + 60.0) / 72.0;
        }
        ImGui::ProgressBar(val, ImVec2(-FLT_MIN, 0), "");
    }

    float voldb     = -60.0;
    char const* lbl = "-inf";
    if (_parameterValues[PARAM_VOLUME] > 0.001) {
        voldb = log(_parameterValues[PARAM_VOLUME]) * 20.0 / log(10.0);
        lbl   = "%+.1f dB";
    }
    ImGui::SliderFloat("Volume", &voldb, -60.0F, 12.0F, lbl, 1.0F);
    if (voldb > -60.0) {
        _parameterValues[PARAM_VOLUME] = pow(10.0, voldb / 20.0);
    } else {
        _parameterValues[PARAM_VOLUME] = 0.0;
    }

    float pan = _parameterValues[PARAM_PAN] * 100.0;
    ImGui::SliderFloat("Pan", &pan, -100.0F, 100.0F, "%+.1f%%", 1.0F);
    _parameterValues[PARAM_PAN] = 0.01 * pan;

    for (int c = 0; c < 2; ++c) {
        double val = 0.0;
        if (_peakOut[c] > 0.001) {
            double const db = log(_peakOut[c]) * 20.0 / log(10.0);
            val             = (db + 60.0) / 72.0;
        }
        ImGui::ProgressBar(val, ImVec2(-FLT_MIN, 0), "");
    }
}

auto GainPlugin::getPreferredSize(uint32_t* width, uint32_t* height) -> bool
{
    *width  = 400;
    *height = 400;
    return true;
}

auto GainPlugin::numParameter() -> uint32_t { return NUM_PARAMS; }

auto GainPlugin::getParameterInfo(uint32_t index, clap_param_info_t* info) -> bool
{
    if (index < 0 || index >= NUM_PARAMS) { return false; }
    *info = param_info[index];
    return true;
}

auto GainPlugin::getParameterValue(clap_id id, double* value) -> bool
{
    if (value == nullptr) { return false; }
    if (id < 0 || id >= NUM_PARAMS) { return false; }

    if (id == PARAM_VOLUME) {
        if (_parameterValues[PARAM_VOLUME] <= 0.0) {
            *value = -150.0;
        } else {
            *value = log(_parameterValues[PARAM_VOLUME]) * 20.0 / log(10.0);
        }
    } else if (id == PARAM_PAN) {
        *value = 100.0 * _parameterValues[PARAM_PAN];
    }
    return true;
}

auto GainPlugin::valueToText(clap_id id, double value, char* display, uint32_t size) -> bool
{
    if ((display == nullptr) || (size == 0U)) { return false; }
    if (id < 0 || id >= NUM_PARAMS) { return false; }

    if (id == PARAM_VOLUME) {
        if (value <= -150.0) {
            strcpy(display, "-inf");
        } else {
            sprintf(display, "%+.2f", value);
        }
    } else if (id == PARAM_PAN) {
        sprintf(display, "%+.0f%%", value);
    }
    return true;
}

auto GainPlugin::textToValue(clap_id id, char const* display, double* value) -> bool
{
    if ((display == nullptr) || (value == nullptr)) { return false; }
    if (id < 0 || id >= NUM_PARAMS) { return false; }

    if (id == PARAM_VOLUME) {
        if (strcmp(display, "-inf") == 0) {
            *value = -150.0;
        } else {
            *value = atof(display);
        }
    } else if (id == PARAM_PAN) {
        *value = atof(display);
    }
    return true;
}

void GainPlugin::flushParameter(clap_input_events const* in, clap_output_events const* /*out*/)
{
    if (in == nullptr) { return; }

    for (int i = 0; i < in->size(in); ++i) {
        auto const* evt = in->get(in, i);
        if ((evt == nullptr) || evt->space_id != CLAP_CORE_EVENT_SPACE_ID) { continue; }
        if (evt->type == CLAP_EVENT_PARAM_VALUE) {
            auto const* pevt = (clap_event_param_value const*)evt;
            if (pevt->param_id < 0 || pevt->param_id >= NUM_PARAMS) {
                continue;  // assert
            }

            _parameterValues[pevt->param_id] = paramConvertValue(pevt->param_id, pevt->value);
        }
    }
}

static char const* GainPluginFeatures[] = {"", nullptr};

static auto GainPluginDescriptor = clap_plugin_descriptor_t{
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

auto getGainPluginDescriptor() -> clap_plugin_descriptor* { return &GainPluginDescriptor; }

auto createGainPlugin(clap_host const* host) -> AudioPlugin* { return new GainPlugin(host); }
