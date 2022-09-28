// passthrough functions.
// add generic passthroughs for extensions here.
// plugin implementations should not need to touch this file.

#include <string.h>

#include "main.h"

clap_host const* g_clap_host;
extern clap_plugin_timer_support gui__timer_support;

static auto asPlugin(clap_plugin const* plugin)
{
    return (Plugin*)(plugin->plugin_data);
}

namespace plugin {
bool init(clap_plugin const* plugin) { return asPlugin(plugin)->init(); }

void destroy(clap_plugin const* plugin) { delete (Plugin*)plugin->plugin_data; }

bool activate(
    clap_plugin const* plugin,
    double sr,
    uint32_t minFrames,
    uint32_t maxFrames
)
{
    return asPlugin(plugin)->activate(sr, minFrames, maxFrames);
}

void deactivate(clap_plugin const* plugin) { asPlugin(plugin)->deactivate(); }

bool start_processing(clap_plugin const* plugin)
{
    return asPlugin(plugin)->startProcessing();
}

void stop_processing(clap_plugin const* plugin)
{
    asPlugin(plugin)->stopProcessing();
}

clap_process_status
process(clap_plugin const* plugin, clap_process const* process)
{
    return asPlugin(plugin)->process(process);
}

void const* get_extension(clap_plugin const* plugin, char const* id)
{
    if (!strcmp(id, CLAP_EXT_GUI)) return &asPlugin(plugin)->m_clap_plugin_gui;
    if (!strcmp(id, CLAP_EXT_TIMER_SUPPORT)) return &gui__timer_support;
    return asPlugin(plugin)->getExtension(id);
}

void on_main_thread(clap_plugin const* plugin)
{
    asPlugin(plugin)->onMainThread();
}
};  // namespace plugin

namespace gui {
bool is_api_supported(
    clap_plugin const* plugin,
    char const* api,
    bool is_floating
)
{
    return asPlugin(plugin)->isApiSupported(api, is_floating);
}

bool create(clap_plugin const* plugin, char const* api, bool is_floating)
{
    return asPlugin(plugin)->createUI(api, is_floating);
}

void destroy(clap_plugin const* plugin) { asPlugin(plugin)->destroyUI(false); }

bool set_scale(clap_plugin const* plugin, double scale)
{
    return asPlugin(plugin)->setScale(scale);
}

bool get_size(clap_plugin const* plugin, uint32_t* width, uint32_t* height)
{
    return asPlugin(plugin)->getSize(width, height);
}

bool can_resize(clap_plugin const* plugin)
{
    return asPlugin(plugin)->canResize();
}

bool adjust_size(clap_plugin const* plugin, uint32_t* width, uint32_t* height)
{
    return asPlugin(plugin)->adjustSize(width, height);
}

bool set_size(clap_plugin const* plugin, uint32_t width, uint32_t height)
{
    return asPlugin(plugin)->setSize(width, height);
}

bool set_parent(clap_plugin const* plugin, clap_window const* window)
{
    return asPlugin(plugin)->setParent(window);
}

bool set_transient(clap_plugin const* plugin, clap_window const* window)
{
    return false;
}

void suggest_title(clap_plugin const* plugin, char const* title)
{
    return asPlugin(plugin)->suggestTitle(title);
}

bool show(clap_plugin const* plugin) { return asPlugin(plugin)->showUI(); }

bool hide(clap_plugin const* plugin) { return asPlugin(plugin)->hideUI(); }
};  // namespace gui

namespace params {
uint32_t count(clap_plugin const* plugin)
{
    return asPlugin(plugin)->numParameter();
}

bool get_info(
    clap_plugin const* plugin,
    uint32_t param_index,
    clap_param_info_t* param_info
)
{
    return asPlugin(plugin)->getParameterInfo(param_index, param_info);
}

bool get_value(clap_plugin const* plugin, clap_id param_id, double* value)
{
    return asPlugin(plugin)->getParameterValue(param_id, value);
}

bool value_to_text(
    clap_plugin const* plugin,
    clap_id param_id,
    double value,
    char* display,
    uint32_t size
)
{
    return asPlugin(plugin)->valueToText(param_id, value, display, size);
}

bool text_to_value(
    clap_plugin const* plugin,
    clap_id param_id,
    char const* display,
    double* value
)
{
    return asPlugin(plugin)->textToValue(param_id, display, value);
}

void flush(
    clap_plugin const* plugin,
    clap_input_events const* in,
    clap_output_events const* out
)
{
    return asPlugin(plugin)->flushParameter(in, out);
}
};  // namespace params

Plugin::Plugin(clap_plugin_descriptor const* descriptor, clap_host const* host)
{
    m_w      = 0;
    m_h      = 0;
    m_ui_ctx = NULL;

    g_clap_host = host;

    m_clap_plugin.desc             = descriptor;
    m_clap_plugin.plugin_data      = this;
    m_clap_plugin.init             = plugin::init;
    m_clap_plugin.destroy          = plugin::destroy;
    m_clap_plugin.activate         = plugin::activate;
    m_clap_plugin.deactivate       = plugin::deactivate;
    m_clap_plugin.start_processing = plugin::start_processing;
    m_clap_plugin.stop_processing  = plugin::stop_processing;
    m_clap_plugin.process          = plugin::process;
    m_clap_plugin.get_extension    = plugin::get_extension;
    m_clap_plugin.on_main_thread   = plugin::on_main_thread;

    m_clap_plugin_gui.is_api_supported = gui::is_api_supported;
    m_clap_plugin_gui.create           = gui::create;
    m_clap_plugin_gui.destroy          = gui::destroy;
    m_clap_plugin_gui.set_scale        = gui::set_scale;
    m_clap_plugin_gui.get_size         = gui::get_size;
    m_clap_plugin_gui.can_resize       = gui::can_resize;
    m_clap_plugin_gui.adjust_size      = gui::adjust_size;
    m_clap_plugin_gui.set_size         = gui::set_size;
    m_clap_plugin_gui.set_parent       = gui::set_parent;
    m_clap_plugin_gui.set_transient    = gui::set_transient;
    m_clap_plugin_gui.suggest_title    = gui::suggest_title;
    m_clap_plugin_gui.show             = gui::show;
    m_clap_plugin_gui.hide             = gui::hide;

    m_clap_plugin_params.count         = params::count;
    m_clap_plugin_params.get_info      = params::get_info;
    m_clap_plugin_params.get_value     = params::get_value;
    m_clap_plugin_params.value_to_text = params::value_to_text;
    m_clap_plugin_params.text_to_value = params::text_to_value;
    m_clap_plugin_params.flush         = params::flush;
}

Plugin::~Plugin() { destroyUI(true); }