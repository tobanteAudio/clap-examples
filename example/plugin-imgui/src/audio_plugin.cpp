// passthrough functions.
// add generic passthroughs for extensions here.
// plugin implementations should not need to touch this file.

#include "audio_plugin.hpp"

#include <cstring>

clap_host const* clapHost;
extern clap_plugin_timer_support guiTimerSupport;

[[nodiscard]] static auto asPlugin(clap_plugin const* plugin) -> AudioPlugin*
{
    return static_cast<AudioPlugin*>(plugin->plugin_data);
}

namespace plugin {
auto init(clap_plugin const* plugin) -> bool { return asPlugin(plugin)->init(); }

void destroy(clap_plugin const* plugin) { delete (AudioPlugin*)plugin->plugin_data; }

auto activate(clap_plugin const* plugin, double sr, uint32_t minFrames, uint32_t maxFrames) -> bool
{
    return asPlugin(plugin)->activate(sr, minFrames, maxFrames);
}

void deactivate(clap_plugin const* plugin) { asPlugin(plugin)->deactivate(); }

auto start_processing(clap_plugin const* plugin) -> bool
{
    return asPlugin(plugin)->startProcessing();
}

void stop_processing(clap_plugin const* plugin) { asPlugin(plugin)->stopProcessing(); }

auto process(clap_plugin const* plugin, clap_process const* process) -> clap_process_status
{
    return asPlugin(plugin)->process(process);
}

auto get_extension(clap_plugin const* plugin, char const* id) -> void const*
{
    if (strcmp(id, CLAP_EXT_GUI) == 0) { return &asPlugin(plugin)->clapGuiHandle; }
    if (strcmp(id, CLAP_EXT_TIMER_SUPPORT) == 0) { return &guiTimerSupport; }
    return asPlugin(plugin)->getExtension(id);
}

void on_main_thread(clap_plugin const* plugin) { asPlugin(plugin)->onMainThread(); }
};  // namespace plugin

namespace gui {
auto is_api_supported(clap_plugin const* plugin, char const* api, bool is_floating) -> bool
{
    return asPlugin(plugin)->isApiSupported(api, is_floating);
}

auto create(clap_plugin const* plugin, char const* api, bool is_floating) -> bool
{
    return asPlugin(plugin)->createUI(api, is_floating);
}

void destroy(clap_plugin const* plugin) { asPlugin(plugin)->destroyUI(false); }

auto set_scale(clap_plugin const* plugin, double scale) -> bool
{
    return asPlugin(plugin)->setScale(scale);
}

auto get_size(clap_plugin const* plugin, uint32_t* width, uint32_t* height) -> bool
{
    return asPlugin(plugin)->getSize(width, height);
}

auto can_resize(clap_plugin const* plugin) -> bool { return asPlugin(plugin)->canResize(); }

auto adjust_size(clap_plugin const* plugin, uint32_t* width, uint32_t* height) -> bool
{
    return asPlugin(plugin)->adjustSize(width, height);
}

auto set_size(clap_plugin const* plugin, uint32_t width, uint32_t height) -> bool
{
    return asPlugin(plugin)->setSize(width, height);
}

auto set_parent(clap_plugin const* plugin, clap_window const* window) -> bool
{
    return asPlugin(plugin)->setParent(window);
}

auto set_transient(clap_plugin const* /*plugin*/, clap_window const* /*window*/) -> bool
{
    return false;
}

void suggest_title(clap_plugin const* plugin, char const* title)
{
    return asPlugin(plugin)->suggestTitle(title);
}

auto show(clap_plugin const* plugin) -> bool { return asPlugin(plugin)->showUI(); }

auto hide(clap_plugin const* plugin) -> bool { return asPlugin(plugin)->hideUI(); }
};  // namespace gui

namespace params {
auto count(clap_plugin const* plugin) -> uint32_t { return asPlugin(plugin)->numParameter(); }

auto get_info(clap_plugin const* plugin, uint32_t param_index, clap_param_info_t* param_info) -> bool
{
    return asPlugin(plugin)->getParameterInfo(param_index, param_info);
}

auto get_value(clap_plugin const* plugin, clap_id param_id, double* value) -> bool
{
    return asPlugin(plugin)->getParameterValue(param_id, value);
}

auto value_to_text(
    clap_plugin const* plugin,
    clap_id param_id,
    double value,
    char* display,
    uint32_t size
) -> bool
{
    return asPlugin(plugin)->valueToText(param_id, value, display, size);
}

auto text_to_value(clap_plugin const* plugin, clap_id param_id, char const* display, double* value)
    -> bool
{
    return asPlugin(plugin)->textToValue(param_id, display, value);
}

void flush(clap_plugin const* plugin, clap_input_events const* in, clap_output_events const* out)
{
    return asPlugin(plugin)->flushParameter(in, out);
}
};  // namespace params

AudioPlugin::AudioPlugin(clap_plugin_descriptor const* descriptor, clap_host const* host)
{

    clapHost = host;

    clapPluginHandle.desc             = descriptor;
    clapPluginHandle.plugin_data      = this;
    clapPluginHandle.init             = plugin::init;
    clapPluginHandle.destroy          = plugin::destroy;
    clapPluginHandle.activate         = plugin::activate;
    clapPluginHandle.deactivate       = plugin::deactivate;
    clapPluginHandle.start_processing = plugin::start_processing;
    clapPluginHandle.stop_processing  = plugin::stop_processing;
    clapPluginHandle.process          = plugin::process;
    clapPluginHandle.get_extension    = plugin::get_extension;
    clapPluginHandle.on_main_thread   = plugin::on_main_thread;

    clapGuiHandle.is_api_supported = gui::is_api_supported;
    clapGuiHandle.create           = gui::create;
    clapGuiHandle.destroy          = gui::destroy;
    clapGuiHandle.set_scale        = gui::set_scale;
    clapGuiHandle.get_size         = gui::get_size;
    clapGuiHandle.can_resize       = gui::can_resize;
    clapGuiHandle.adjust_size      = gui::adjust_size;
    clapGuiHandle.set_size         = gui::set_size;
    clapGuiHandle.set_parent       = gui::set_parent;
    clapGuiHandle.set_transient    = gui::set_transient;
    clapGuiHandle.suggest_title    = gui::suggest_title;
    clapGuiHandle.show             = gui::show;
    clapGuiHandle.hide             = gui::hide;

    clapParameterHandle.count         = params::count;
    clapParameterHandle.get_info      = params::get_info;
    clapParameterHandle.get_value     = params::get_value;
    clapParameterHandle.value_to_text = params::value_to_text;
    clapParameterHandle.text_to_value = params::text_to_value;
    clapParameterHandle.flush         = params::flush;
}

AudioPlugin::~AudioPlugin() { destroyUI(true); }
