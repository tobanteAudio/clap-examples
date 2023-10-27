// generic ui functions.
// plugin implementations that use imgui should not need to touch this file.

#include "audio_plugin.hpp"

#include <cstdio>
#include <cstring>

auto AudioPlugin::createUI(char const* api, bool is_floating) -> bool
{
    return isApiSupported(api, is_floating);
}

auto AudioPlugin::setScale(double /*scale*/) -> bool { return false; }

auto AudioPlugin::getSize(uint32_t* width, uint32_t* height) -> bool
{
    if (windowWidth > 0 && windowHeight > 0) {
        *width  = windowWidth;
        *height = windowHeight;
    } else {
        getPreferredSize(width, height);
    }
    return true;
}

auto AudioPlugin::canResize() -> bool { return true; }

auto AudioPlugin::adjustSize(uint32_t* /*width*/, uint32_t* /*height*/) -> bool { return true; }

auto AudioPlugin::setSize(uint32_t width, uint32_t height) -> bool
{
    // imgui should respond dynamically to the host window size changing
    windowWidth  = width;
    windowHeight = height;
    return true;
}

auto AudioPlugin::setTransient(clap_window const* /*window*/) -> bool { return false; }

void AudioPlugin::suggestTitle(char const* title) {}

auto AudioPlugin::showUI() -> bool { return true; }

auto AudioPlugin::hideUI() -> bool { return true; }
