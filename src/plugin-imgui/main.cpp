#include <cstdlib>
#include <stdint.h>
#include <string.h>

#if defined _WIN32
#include <windows.h>
HINSTANCE g_hinst;
#endif

#include "main.h"

namespace factory {
auto getPluginCount(clap_plugin_factory const* factory) -> uint32_t
{
    return 2;
}

auto getPluginDescriptor(clap_plugin_factory const* factory, uint32_t index)
    -> clap_plugin_descriptor const*
{
    if (index == 0) return getGainPluginDescriptor();
    return NULL;
}

auto createPlugin(
    clap_plugin_factory const* factory,
    clap_host const* host,
    char const* pluginID
) -> clap_plugin const*
{
    Plugin* plugin = NULL;
    if (!strcmp(getGainPluginDescriptor()->id, pluginID))
        plugin = createGainPlugin(host);
    if (plugin) return &plugin->m_clap_plugin;
    return NULL;
}

}  // namespace factory

clap_plugin_factory plugin_factory = {
    factory::getPluginCount,
    factory::getPluginDescriptor,
    factory::createPlugin,
};

namespace entry {
auto init(char const* path) -> bool { return true; }

auto deinit() -> void {}

auto getFactory(char const* factoryID) -> void const*
{
    return factoryID && !strcmp(factoryID, CLAP_PLUGIN_FACTORY_ID)
             ? &plugin_factory
             : nullptr;
}
};  // namespace entry

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