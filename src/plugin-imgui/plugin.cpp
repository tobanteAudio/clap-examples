// passthrough functions.
// add generic passthroughs for extensions here.
// plugin implementations should not need to touch this file.

#include "main.h"
#include <string.h>

const clap_host *g_clap_host;
extern clap_plugin_timer_support gui__timer_support;

static auto asPlugin(const clap_plugin *plugin) {
  return (Plugin *)(plugin->plugin_data);
}

namespace plugin {
bool init(const clap_plugin *plugin) { return asPlugin(plugin)->init(); }
void destroy(const clap_plugin *plugin) {
  delete (Plugin *)plugin->plugin_data;
}
bool activate(const clap_plugin *plugin, double sample_rate,
              uint32_t min_frames_count, uint32_t max_frames_count) {
  return asPlugin(plugin)->activate(sample_rate, min_frames_count,
                                    max_frames_count);
}
void deactivate(const clap_plugin *plugin) { asPlugin(plugin)->deactivate(); }
bool start_processing(const clap_plugin *plugin) {
  return asPlugin(plugin)->startProcessing();
}
void stop_processing(const clap_plugin *plugin) {
  asPlugin(plugin)->stopProcessing();
}
clap_process_status process(const clap_plugin *plugin,
                            const clap_process *process) {
  return asPlugin(plugin)->process(process);
}
const void *get_extension(const clap_plugin *plugin, const char *id) {
  if (!strcmp(id, CLAP_EXT_GUI))
    return &asPlugin(plugin)->m_clap_plugin_gui;
  if (!strcmp(id, CLAP_EXT_TIMER_SUPPORT))
    return &gui__timer_support;
  return asPlugin(plugin)->getExtension(id);
}
void on_main_thread(const clap_plugin *plugin) {
  asPlugin(plugin)->onMainThread();
}
}; // namespace plugin

namespace gui {
bool is_api_supported(const clap_plugin *plugin, const char *api,
                      bool is_floating) {
  return asPlugin(plugin)->isApiSupported(api, is_floating);
}
bool create(const clap_plugin *plugin, const char *api, bool is_floating) {
  return asPlugin(plugin)->createUI(api, is_floating);
}
void destroy(const clap_plugin *plugin) { asPlugin(plugin)->destroyUI(false); }
bool set_scale(const clap_plugin *plugin, double scale) {
  return asPlugin(plugin)->setScale(scale);
}
bool get_size(const clap_plugin *plugin, uint32_t *width, uint32_t *height) {
  return asPlugin(plugin)->getSize(width, height);
}
bool can_resize(const clap_plugin *plugin) {
  return asPlugin(plugin)->canResize();
}
bool adjust_size(const clap_plugin *plugin, uint32_t *width, uint32_t *height) {
  return asPlugin(plugin)->adjustSize(width, height);
}
bool set_size(const clap_plugin *plugin, uint32_t width, uint32_t height) {
  return asPlugin(plugin)->setSize(width, height);
}
bool set_parent(const clap_plugin *plugin, const clap_window *window) {
  return asPlugin(plugin)->setParent(window);
}
bool set_transient(const clap_plugin *plugin, const clap_window *window) {
  return false;
}
void suggest_title(const clap_plugin *plugin, const char *title) {
  return asPlugin(plugin)->suggestTitle(title);
}
bool show(const clap_plugin *plugin) { return asPlugin(plugin)->showUI(); }
bool hide(const clap_plugin *plugin) { return asPlugin(plugin)->hideUI(); }
}; // namespace gui

namespace params {
uint32_t count(const clap_plugin *plugin) {
  return asPlugin(plugin)->params__count();
}
bool get_info(const clap_plugin *plugin, uint32_t param_index,
              clap_param_info_t *param_info) {
  return asPlugin(plugin)->getParameterInfo(param_index, param_info);
}
bool get_value(const clap_plugin *plugin, clap_id param_id, double *value) {
  return asPlugin(plugin)->getParameterValue(param_id, value);
}
bool value_to_text(const clap_plugin *plugin, clap_id param_id, double value,
                   char *display, uint32_t size) {
  return asPlugin(plugin)->valueToText(param_id, value, display, size);
}
bool text_to_value(const clap_plugin *plugin, clap_id param_id,
                   const char *display, double *value) {
  return asPlugin(plugin)->textToValue(param_id, display, value);
}
void flush(const clap_plugin *plugin, const clap_input_events *in,
           const clap_output_events *out) {
  return asPlugin(plugin)->flushParameter(in, out);
}
}; // namespace params

Plugin::Plugin(const clap_plugin_descriptor *descriptor,
               const clap_host *host) {
  m_w = 0;
  m_h = 0;
  m_ui_ctx = NULL;

  g_clap_host = host;

  m_clap_plugin.desc = descriptor;
  m_clap_plugin.plugin_data = this;
  m_clap_plugin.init = plugin::init;
  m_clap_plugin.destroy = plugin::destroy;
  m_clap_plugin.activate = plugin::activate;
  m_clap_plugin.deactivate = plugin::deactivate;
  m_clap_plugin.start_processing = plugin::start_processing;
  m_clap_plugin.stop_processing = plugin::stop_processing;
  m_clap_plugin.process = plugin::process;
  m_clap_plugin.get_extension = plugin::get_extension;
  m_clap_plugin.on_main_thread = plugin::on_main_thread;

  m_clap_plugin_gui.is_api_supported = gui::is_api_supported;
  m_clap_plugin_gui.create = gui::create;
  m_clap_plugin_gui.destroy = gui::destroy;
  m_clap_plugin_gui.set_scale = gui::set_scale;
  m_clap_plugin_gui.get_size = gui::get_size;
  m_clap_plugin_gui.can_resize = gui::can_resize;
  m_clap_plugin_gui.adjust_size = gui::adjust_size;
  m_clap_plugin_gui.set_size = gui::set_size;
  m_clap_plugin_gui.set_parent = gui::set_parent;
  m_clap_plugin_gui.set_transient = gui::set_transient;
  m_clap_plugin_gui.suggest_title = gui::suggest_title;
  m_clap_plugin_gui.show = gui::show;
  m_clap_plugin_gui.hide = gui::hide;

  m_clap_plugin_params.count = params::count;
  m_clap_plugin_params.get_info = params::get_info;
  m_clap_plugin_params.get_value = params::get_value;
  m_clap_plugin_params.value_to_text = params::value_to_text;
  m_clap_plugin_params.text_to_value = params::text_to_value;
  m_clap_plugin_params.flush = params::flush;
}

Plugin::~Plugin() { destroyUI(true); }