// This file is here to demonstrate how to wire a CLAP plugin
// You can use it as a starting point, however if you are implementing a C++
// plugin, I'd encourage you to use the C++ glue layer instead:
// https://github.com/free-audio/clap-helpers/blob/main/include/clap/helpers/plugin.hh

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <clap/clap.h>

static constexpr auto features = std::array<char const*, 3>{
    CLAP_PLUGIN_FEATURE_INSTRUMENT,
    CLAP_PLUGIN_FEATURE_STEREO,
    nullptr,
};

static clap_plugin_descriptor_t const s_my_plug_desc = {
    .clap_version = CLAP_VERSION,
    .id           = "com.tobanteAudio.clap-minimal",
    .name         = "CLAP Minimal",
    .vendor       = "tobanteAudio",
    .url          = "https://github.com/tobanteAudio/clap-examples",
    .manual_url   = "https://github.com/tobanteAudio/clap-examples",
    .support_url  = "https://github.com/tobanteAudio/clap-examples",
    .version      = "0.1.0",
    .description  = "Minimal CLAP Audio-Plugin",
    .features     = (char const**)(features.data()),
};

using my_plug_t = struct
{
    clap_plugin_t plugin;
    clap_host_t const* host;
    clap_host_latency_t const* hostLatency;
    clap_host_log_t const* hostLog;
    clap_host_thread_check_t const* hostThreadCheck;

    uint32_t latency;
};

/////////////////////////////
// clap_plugin_audio_ports //
/////////////////////////////

static auto my_plug_audio_ports_count(clap_plugin_t const* /*plugin*/, bool /*is_input*/) -> uint32_t
{
    return 1;
}

static auto my_plug_audio_ports_get(
    clap_plugin_t const* /*plugin*/,
    uint32_t index,
    bool /*is_input*/,
    clap_audio_port_info_t* info
) -> bool
{
    if (index > 0) { return false; }
    info->id = 0;
    snprintf(info->name, sizeof(info->name), "%s", "My Port Name");
    info->channel_count = 2;
    info->flags         = CLAP_AUDIO_PORT_IS_MAIN;
    info->port_type     = CLAP_PORT_STEREO;
    info->in_place_pair = CLAP_INVALID_ID;
    return true;
}

static clap_plugin_audio_ports_t const s_my_plug_audio_ports = {
    .count = my_plug_audio_ports_count,
    .get   = my_plug_audio_ports_get,
};

////////////////////////////
// clap_plugin_note_ports //
////////////////////////////

static auto my_plug_note_ports_count(clap_plugin_t const* /*plugin*/, bool /*is_input*/) -> uint32_t
{
    return 1;
}

static auto my_plug_note_ports_get(
    clap_plugin_t const* /*plugin*/,
    uint32_t index,
    bool /*is_input*/,
    clap_note_port_info_t* info
) -> bool
{
    if (index > 0) { return false; }
    info->id = 0;
    snprintf(info->name, sizeof(info->name), "%s", "My Port Name");
    info->supported_dialects
        = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI_MPE | CLAP_NOTE_DIALECT_MIDI2;
    info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
    return true;
}

static clap_plugin_note_ports_t const s_my_plug_note_ports = {
    .count = my_plug_note_ports_count,
    .get   = my_plug_note_ports_get,
};

//////////////////
// clap_latency //
//////////////////

auto my_plug_latency_get(clap_plugin_t const* plugin) -> uint32_t
{
    auto* plug = (my_plug_t*)plugin->plugin_data;
    return plug->latency;
}

static clap_plugin_latency_t const s_my_plug_latency = {
    .get = my_plug_latency_get,
};

/////////////////
// clap_plugin //
/////////////////

static auto my_plug_init(const struct clap_plugin* plugin) -> bool
{
    auto* plug = (my_plug_t*)plugin->plugin_data;

    // Fetch host's extensions here
    plug->hostLog = (clap_host_log_t*)plug->host->get_extension(plug->host, CLAP_EXT_LOG);
    plug->hostThreadCheck
        = (clap_host_thread_check_t*)plug->host->get_extension(plug->host, CLAP_EXT_THREAD_CHECK);
    plug->hostLatency = (clap_host_latency_t*)plug->host->get_extension(plug->host, CLAP_EXT_LATENCY);
    return true;
}

static void my_plug_destroy(const struct clap_plugin* plugin)
{
    auto* plug = (my_plug_t*)plugin->plugin_data;
    free(plug);
}

static auto my_plug_activate(
    const struct clap_plugin* /*plugin*/,
    double /*sr*/,
    uint32_t /*minFrames*/,
    uint32_t /*maxFrames*/
) -> bool
{
    return true;
}

static void my_plug_deactivate(const struct clap_plugin* plugin) {}

static auto my_plug_start_processing(const struct clap_plugin* /*plugin*/) -> bool { return true; }

static void my_plug_stop_processing(const struct clap_plugin* plugin) {}

static void my_plug_reset(const struct clap_plugin* plugin) {}

static void my_plug_process_event(clap_event_header_t const* hdr)
{
    if (hdr->space_id == CLAP_CORE_EVENT_SPACE_ID) {
        switch (hdr->type) {
            case CLAP_EVENT_NOTE_ON: {
                auto const* ev = (clap_event_note_t const*)hdr;
                // TODO: handle note on
                break;
            }

            case CLAP_EVENT_NOTE_OFF: {
                auto const* ev = (clap_event_note_t const*)hdr;
                // TODO: handle note off
                break;
            }

            case CLAP_EVENT_NOTE_CHOKE: {
                auto const* ev = (clap_event_note_t const*)hdr;
                // TODO: handle note choke
                break;
            }

            case CLAP_EVENT_NOTE_EXPRESSION: {
                auto const* ev = (clap_event_note_expression_t const*)hdr;
                // TODO: handle note expression
                break;
            }

            case CLAP_EVENT_PARAM_VALUE: {
                auto const* ev = (clap_event_param_value_t const*)hdr;
                // TODO: handle parameter change
                break;
            }

            case CLAP_EVENT_PARAM_MOD: {
                auto const* ev = (clap_event_param_mod_t const*)hdr;
                // TODO: handle parameter modulation
                break;
            }

            case CLAP_EVENT_TRANSPORT: {
                auto const* ev = (clap_event_transport_t const*)hdr;
                // TODO: handle transport event
                break;
            }

            case CLAP_EVENT_MIDI: {
                auto const* ev = (clap_event_midi_t const*)hdr;
                // TODO: handle MIDI event
                break;
            }

            case CLAP_EVENT_MIDI_SYSEX: {
                auto const* ev = (clap_event_midi_sysex_t const*)hdr;
                // TODO: handle MIDI Sysex event
                break;
            }

            case CLAP_EVENT_MIDI2: {
                auto const* ev = (clap_event_midi2_t const*)hdr;
                // TODO: handle MIDI2 event
                break;
            }
        }
    }
}

static auto my_plug_process(const struct clap_plugin* plugin, clap_process_t const* process)
    -> clap_process_status
{
    auto* plug             = (my_plug_t*)plugin->plugin_data;
    uint32_t const nframes = process->frames_count;
    uint32_t const nev     = process->in_events->size(process->in_events);
    uint32_t ev_index      = 0;
    uint32_t next_ev_frame = nev > 0 ? 0 : nframes;

    for (uint32_t i = 0; i < nframes;) {
        /* handle every events that happrens at the frame "i" */
        while (ev_index < nev && next_ev_frame == i) {
            clap_event_header_t const* hdr = process->in_events->get(process->in_events, ev_index);
            if (hdr->time != i) {
                next_ev_frame = hdr->time;
                break;
            }

            my_plug_process_event(hdr);
            ++ev_index;

            if (ev_index == nev) {
                // we reached the end of the event list
                next_ev_frame = nframes;
                break;
            }
        }

        /* process every samples until the next event */
        for (; i < next_ev_frame; ++i) {
            // fetch input samples
            float const in_l = process->audio_inputs[0].data32[0][i];
            float const in_r = process->audio_inputs[0].data32[1][i];

            /* TODO: process samples, here we simply swap left and right
             * channels */
            float const out_l = in_r;
            float const out_r = in_l;

            // store output samples
            process->audio_outputs[0].data32[0][i] = out_l;
            process->audio_outputs[0].data32[1][i] = out_r;
        }
    }

    return CLAP_PROCESS_CONTINUE;
}

static auto my_plug_get_extension(const struct clap_plugin* /*plugin*/, char const* id) -> void const*
{
    if (strcmp(id, CLAP_EXT_LATENCY) == 0) { return &s_my_plug_latency; }
    if (strcmp(id, CLAP_EXT_AUDIO_PORTS) == 0) { return &s_my_plug_audio_ports; }
    if (strcmp(id, CLAP_EXT_NOTE_PORTS) == 0) { return &s_my_plug_note_ports; }
    // TODO: add support to CLAP_EXT_PARAMS
    // TODO: add support to CLAP_EXT_STATE
    return nullptr;
}

static void my_plug_on_main_thread(const struct clap_plugin* plugin) {}

auto my_plug_create(clap_host_t const* host) -> clap_plugin_t*
{
    auto* p                    = (my_plug_t*)calloc(1, sizeof(my_plug_t));
    p->host                    = host;
    p->plugin.desc             = &s_my_plug_desc;
    p->plugin.plugin_data      = p;
    p->plugin.init             = my_plug_init;
    p->plugin.destroy          = my_plug_destroy;
    p->plugin.activate         = my_plug_activate;
    p->plugin.deactivate       = my_plug_deactivate;
    p->plugin.start_processing = my_plug_start_processing;
    p->plugin.stop_processing  = my_plug_stop_processing;
    p->plugin.reset            = my_plug_reset;
    p->plugin.process          = my_plug_process;
    p->plugin.get_extension    = my_plug_get_extension;
    p->plugin.on_main_thread   = my_plug_on_main_thread;

    // Don't call into the host here

    return &p->plugin;
}

/////////////////////////
// clap_plugin_factory //
/////////////////////////

static struct
{
    clap_plugin_descriptor_t const* desc;
    clap_plugin_t* (*create)(clap_host_t const* host);
} s_plugins[] = {
    {
     .desc   = &s_my_plug_desc,
     .create = my_plug_create,
     },
};

static auto plugin_factory_get_plugin_count(const struct clap_plugin_factory* /*factory*/) -> uint32_t
{
    return sizeof(s_plugins) / sizeof(s_plugins[0]);
}

static auto
plugin_factory_get_plugin_descriptor(const struct clap_plugin_factory* /*factory*/, uint32_t index)
    -> clap_plugin_descriptor_t const*
{
    return s_plugins[index].desc;
}

static auto plugin_factory_create_plugin(
    const struct clap_plugin_factory* /*factory*/,
    clap_host_t const* host,
    char const* plugin_id
) -> clap_plugin_t const*
{
    if (!clap_version_is_compatible(host->clap_version)) { return nullptr; }

    int const N = sizeof(s_plugins) / sizeof(s_plugins[0]);
    for (auto& s_plugin : s_plugins) {
        if (strcmp(plugin_id, s_plugin.desc->id) == 0) { return s_plugin.create(host); }
    }

    return nullptr;
}

static clap_plugin_factory_t const s_plugin_factory = {
    .get_plugin_count      = plugin_factory_get_plugin_count,
    .get_plugin_descriptor = plugin_factory_get_plugin_descriptor,
    .create_plugin         = plugin_factory_create_plugin,
};

////////////////
// clap_entry //
////////////////

static auto entry_init(char const* /*plugin_path*/) -> bool
{
    // called only once, and very first
    return true;
}

static void entry_deinit()
{
    // called before unloading the DSO
}

static auto entry_get_factory(char const* factory_id) -> void const*
{
    if (strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) == 0) { return &s_plugin_factory; }
    return nullptr;
}

CLAP_EXPORT const clap_plugin_entry_t clap_entry = {
    .clap_version = CLAP_VERSION_INIT,
    .init         = entry_init,
    .deinit       = entry_deinit,
    .get_factory  = entry_get_factory,
};
