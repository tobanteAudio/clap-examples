// generic ui functions.
// plugin implementations that use imgui should not need to touch this file.

#include <stdio.h>
#include <string.h>

#include "main.h"

bool Plugin::createUI(char const* api, bool is_floating)
{
    return isApiSupported(api, is_floating);
}

bool Plugin::setScale(double scale) { return false; }

bool Plugin::getSize(uint32_t* width, uint32_t* height)
{
    if (m_w > 0 && m_h > 0) {
        *width  = m_w;
        *height = m_h;
    } else
        getPreferredSize(width, height);
    return true;
}

bool Plugin::canResize() { return true; }

bool Plugin::adjustSize(uint32_t* width, uint32_t* height) { return true; }

bool Plugin::setSize(uint32_t width, uint32_t height)
{
    // imgui should respond dynamically to the host window size changing
    m_w = width;
    m_h = height;
    return true;
}

bool Plugin::setTransient(clap_window const* window) { return false; }

void Plugin::suggestTitle(char const* title) {}

bool Plugin::showUI() { return true; }

bool Plugin::hideUI() { return true; }