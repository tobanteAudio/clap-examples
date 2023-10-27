#if defined _WIN32
#include <windows.h>
HINSTANCE g_hinst;
#endif

#include "gain_plugin.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string_view>

namespace factory {
auto getPluginCount(clap_plugin_factory const* /*factory*/) -> uint32_t { return 1; }

auto getPluginDescriptor(clap_plugin_factory const* /*factory*/, uint32_t index)
    -> clap_plugin_descriptor const*
{
    if (index == 0) { return getGainPluginDescriptor(); }
    return nullptr;
}

auto create(clap_plugin_factory const* /*factory*/, clap_host const* host, char const* id)
    -> clap_plugin const*
{
    AudioPlugin* plugin = nullptr;
    if (strcmp(getGainPluginDescriptor()->id, id) == 0) { plugin = createGainPlugin(host); }
    if (plugin != nullptr) { return &plugin->clapPluginHandle; }
    return nullptr;
}

}  // namespace factory

clap_plugin_factory plugin_factory = {
    factory::getPluginCount,
    factory::getPluginDescriptor,
    factory::create,
};

namespace entry {
auto init(char const* /*path*/) -> bool { return true; }

auto deinit() -> void {}

auto getFactory(char const* id) -> void const*
{
    auto const factoryID = std::string_view{CLAP_PLUGIN_FACTORY_ID};
    if (id == nullptr) { return nullptr; }
    if (std::string_view{id} == factoryID) { return &plugin_factory; }
    return nullptr;
}
}  // namespace entry

extern "C" {
CLAP_EXPORT const clap_plugin_entry clap_entry = {
    CLAP_VERSION,
    entry::init,
    entry::deinit,
    entry::getFactory,
};

#if defined _WIN32
BOOL WINAPI DllMain(HINSTANCE hDllInst, DWORD fdwReason, LPVOID res)
{
    if (fdwReason == DLL_PROCESS_ATTACH) { g_hinst = hDllInst; }
    return TRUE;
}
#endif
};
