// add generic support for extensions here.
// plugin implementations should not need to touch this file.

#pragma once

#include <clap/clap.h>

struct Plugin {
  clap_plugin m_clap_plugin;
  clap_plugin_params m_clap_plugin_params;
  int m_w, m_h;

  clap_plugin_gui m_clap_plugin_gui;
  void *m_ui_ctx;

  Plugin(const clap_plugin_descriptor *descriptor, const clap_host *host);
  virtual ~Plugin();

  virtual bool init() = 0;
  virtual bool activate(double sample_rate, uint32_t min_frames_count,
                        uint32_t max_frames_count) = 0;
  virtual void deactivate() = 0;
  virtual bool startProcessing() = 0;
  virtual void stopProcessing() = 0;
  virtual clap_process_status process(const clap_process *process) = 0;
  virtual const void *getExtension(const char *id) = 0;
  virtual void onMainThread() = 0;
  virtual void draw() = 0;
  virtual bool getPreferredSize(uint32_t *width, uint32_t *height) = 0;

  bool isApiSupported(const char *api, bool is_floating);
  bool createUI(const char *api, bool is_floating);
  bool destroyUI(bool is_plugin_destroy);
  bool setScale(double scale);
  bool getSize(uint32_t *width, uint32_t *height);
  bool canResize();
  bool adjustSize(uint32_t *width, uint32_t *height);
  bool setSize(uint32_t width, uint32_t height);
  bool setParent(const clap_window *window);
  bool setTransient(const clap_window *window);
  void suggestTitle(const char *title);
  bool showUI();
  bool hideUI();

  virtual uint32_t params__count() = 0;
  virtual bool getParameterInfo(uint32_t param_index,
                                clap_param_info_t *param_info) = 0;
  virtual bool getParameterValue(clap_id param_id, double *value) = 0;
  virtual bool valueToText(clap_id param_id, double value, char *display,
                           uint32_t size) = 0;
  virtual bool textToValue(clap_id param_id, const char *display,
                           double *value) = 0;
  virtual void flushParameter(const clap_input_events *in,
                              const clap_output_events *out) = 0;
};

clap_plugin_descriptor *plugin_impl__get_descriptor_0();
Plugin *plugin_impl__create_0(const clap_host *host);
