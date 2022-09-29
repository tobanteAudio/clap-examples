// generic ui functions.
// plugin implementations that use imgui should not need to touch this file.

#include <stdio.h>
#include <string.h>

#include "audio_plugin.hpp"

bool AudioPlugin::createUI(char const* api, bool is_floating)
{
    return isApiSupported(api, is_floating);
}

bool AudioPlugin::setScale(double scale) { return false; }

bool AudioPlugin::getSize(uint32_t* width, uint32_t* height)
{
    if (m_w > 0 && m_h > 0) {
        *width  = m_w;
        *height = m_h;
    } else {
        getPreferredSize(width, height);
    }
    return true;
}

bool AudioPlugin::canResize() { return true; }

bool AudioPlugin::adjustSize(uint32_t* width, uint32_t* height) { return true; }

bool AudioPlugin::setSize(uint32_t width, uint32_t height)
{
    // imgui should respond dynamically to the host window size changing
    m_w = width;
    m_h = height;
    return true;
}

bool AudioPlugin::setTransient(clap_window const* window) { return false; }

void AudioPlugin::suggestTitle(char const* title) {}

bool AudioPlugin::showUI() { return true; }

bool AudioPlugin::hideUI() { return true; }
