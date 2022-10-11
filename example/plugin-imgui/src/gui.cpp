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
    if (windowWidth > 0 && windowHeight > 0) {
        *width  = windowWidth;
        *height = windowHeight;
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
    windowWidth  = width;
    windowHeight = height;
    return true;
}

bool AudioPlugin::setTransient(clap_window const* window) { return false; }

void AudioPlugin::suggestTitle(char const* title) {}

bool AudioPlugin::showUI() { return true; }

bool AudioPlugin::hideUI() { return true; }
