// add generic support for extensions here.
// plugin implementations should not need to touch this file.

#pragma once

#include "audio_plugin.hpp"

auto getGainPluginDescriptor() -> clap_plugin_descriptor*;
auto createGainPlugin(clap_host const* host) -> AudioPlugin*;
