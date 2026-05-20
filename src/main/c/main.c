// main.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - MAIN APPLICATION ENTRY POINT (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// !!! MUST CALL ciao_output_init() VERY EARLY !!!
//
// This file is PURE ORCHESTRATION only.
// No UI construction, no WebKit logic, no menu building belongs here.
//
// Important CIAO Design Decision (Project-Specific Naming):
// 
//   ui_window_app.*  and  ui_menu_app.*  are deliberately suffixed with "_app".
// 
//   Reason:
//     This project is Github-specific. Other similar projects (YouTube client,
//     GitLab client, etc.) will have different features, menu structures,
//     default pages, and behaviors.
//
//     Therefore, it is better to maintain separate file names per project
//     (ui_window_app.c, ui_menu_app.c) rather than using generic
//     ui_window.c / ui_menu.c which would cause confusion or forced merging
//     when forking for other services.
//
// Protection Rule (Sacred - Do Not Violate):
//   Future AI or developers MUST NOT rename ui_window_app.* or
//   ui_menu_app.* to generic names (ui_window.*, ui_menu.*).
//   Each project should keep its own clearly named UI modules.
// 
//   This follows CIAO Principle: "Explicit Project Intent & Maintainability
//   Across Forks".
//
// New Feature (2026-04-30):
//   Support for --shutdown=N and SHUTDOWN=N environment variable.
//   Added in main.c (orchestration layer) to keep output.c clean.
//
// Last aligned with SyncPrjs CIAO style + ui_window_app + ui_menu_app + 
// Auto-Shutdown feature: 2026-04-30
// =========================================================================

#include "config.h"          // ← Add this
#include "output.h"
#include "project.h"
#include "webkit_setup.h"
#include "ui_window_app.h"
#include <gtk/gtk.h>                    // Required for gtk_main_quit()

// ──────────────────────────────────────────────
// on_auto_shutdown() - CIAO Auto-Shutdown Timer Callback
// ──────────────────────────────────────────────
//
// Purpose:
//   Gracefully terminates the GTK main loop when the shutdown timer expires.
//   Works regardless of page loading state or user activity.
//
// Parameters:
//   Input:
//     - data : gpointer - User data (unused in this implementation)
//   Output/Return:
//     - gboolean - Always G_SOURCE_REMOVE (one-shot timer)
//
// Dependencies:
//   - Functions called: ciao_info(), gtk_main_quit()
//   - Headers required: gtk/gtk.h, output.h
//   - External: Connected via g_timeout_add_seconds() in main()
//
// Design Rules:
//   - Must be static (file-local)
//   - Called via g_timeout_add_seconds() from main orchestration layer
//   - Uses ciao_* logging only
//
// CIAO Principles Applied:
//   - Caution: Proper timer cleanup with G_SOURCE_REMOVE
//   - Intentional: Centralized shutdown logic in orchestration layer
//   - Anti-fragile: Works independently of page state
//   - Over-protect: Dedicated callback for auto-shutdown feature
//
// Protection Rule (Sacred):
//   Do NOT move this callback to output.c or any other module.
//   It belongs here in the pure orchestration file.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
static gboolean on_auto_shutdown(gpointer data)
{
    (void)data;

    ciao_info("SHUTDOWN timer reached — terminating application now.");
    
    gtk_main_quit();           // Must be called from within the main loop
    
    return G_SOURCE_REMOVE;    // Do not repeat the timer
}


// ──────────────────────────────────────────────
// main() - Application Entry Point
// ──────────────────────────────────────────────
//
// Purpose:
//   Main application entry point. Orchestrates initialization, auto-shutdown
//   timer, WebKit setup, UI creation, and GTK main loop. This is the
//   Single Point of Truth for application startup sequence.
//
// Parameters:
//   Input:
//     - argc : int - Number of command line arguments
//     - argv : char** - Command line argument array
//   Output/Return:
//     - int - Exit status (0 = success, 1 = critical failure)
//
// Dependencies:
//   - Functions called: ciao_output_init(), ciao_info(), ciao_debug(), ciao_error(),
//     gtk_init(), setup_webkit_webview(), create_main_window(), gtk_widget_show_all(),
//     gtk_main(), on_auto_shutdown()
//   - Headers required: gtk/gtk.h, output.h, project.h, webkit_setup.h, ui_window_app.h
//   - External: Relies on environment variables and command-line flags (--shutdown, --bookmark, etc.)
//
// Lessons Learned (Critical - Google Translate TTS Audio Issues):
//   Google Translate's audio issue on WebKitGTK arises from a Web Audio API
//   misinterpretation, where raw audio streams (audio/x-raw) are incorrectly
//   processed as video data by GStreamer, causing buffer starvation and
//   robotic distortion.
//
//   What has been tried (environment variables in main()):
//   - Multiple GST_PLUGIN_FEATURE_RANK variations (pipewiresink:MAX, pulsesink:MAX,
//     autoaudiosink:NONE, alsasink:NONE, etc.)
//   - Low-latency tuning (PULSE_LATENCY_MSEC, GST_PULSE_NO_LATENCY)
//   - Resampling and quality flags (GST_AUDIO_RESAMPLER_QUALITY, WEBKIT_GST_AUDIO_RESAMPLING)
//   - VA-API / DMABuf / compositing bypasses (GST_VAAPI_ALL_DRIVERS=0,
//     WEBKIT_DISABLE_DMABUF_RENDERER=1, WEBKIT_DISABLE_COMPOSITING_MODE=1)
//   - Forcing ALSA or pure PipeWire paths
//
//   Why these fixes ultimately failed:
//   - They could temporarily reduce some caps errors or restore sound, but the
//     core architectural bug is hardcoded inside WebKitGTK’s C++ source code
//     (AudioDestinationGStreamer.cpp and MediaPlayerPrivateGStreamer).
//   - WebKit unconditionally wraps Web Audio streams in a generic media player
//     context that attaches a video sink canvas (webkit-gl-video-appsink),
//     triggering video-info.c type mismatches and buffer pool starvation.
//   - Environment variables can only mitigate symptoms; they cannot fix the
//     internal pipeline construction logic in WebKitGTK.
//
//   Conclusion:
//   This is a structural limitation in the current WebKitGTK engine. A permanent
//   fix would require patching and recompiling WebKitGTK itself.
//
// Why This Design:
//   Pure orchestration layer — environment variables must be set very early.
//
// Why This Design:
//   Pure orchestration layer — no UI or WebKit logic belongs here.
//   Ensures output system is initialized first and auto-shutdown is scheduled early.
//
// CIAO Principles Applied:
//   - Caution: Critical failure checks after WebKit and window creation
//   - Intentional: Clear startup sequence with section comments
//   - Anti-fragile: Graceful exit on initialization failures
//   - Over-protect: Auto-shutdown and output init protected in main orchestration
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Move these g_setenv calls after gtk_init() or WebKit setup
//     - Remove or simplify the audio routing overrides
//     - Move auto-shutdown logic outside main()
//     - Remove ciao_output_init() as first step
//     - Add UI/WebKit construction code into this file
//   This section is protected due to hard-learned Web Audio quirks.
//   This file must remain pure orchestration.
//
// Last updated: 2026-05-20 (ALSA routing + pipewiresink:NONE for final caps bug fix)
// ──────────────────────────────────────────────
int main(int argc, char *argv[])
{
    // ====================================================================
    // Force high-quality GStreamer audio routing for Web Audio (Google TTS)
    // ====================================================================
    g_setenv("AUDIODEV", "default", TRUE);
    
    // FIXES CHOPPY AUDIO & QUEUE EMPTY ERRORS:
    // Force autoaudiosink to bypass the buggy native PipeWire auto-wrapper.
    // Hide pipewiresink from WebKit's automatic fallback lookups.
    g_setenv("GST_PLUGIN_FEATURE_RANK", "pulsesink:MAX,autoaudiosink:MAX,pipewiresink:NONE,alsasink:NONE", TRUE);
    
    // Low-latency and audio quality optimization parameters
    g_setenv("GST_AUDIO_RESAMPLER_QUALITY", "10", TRUE);
    g_setenv("PULSE_LATENCY_MSEC", "15", TRUE);            // Aggressive real-time buffer window
    g_setenv("GST_PULSE_NO_LATENCY", "1", TRUE);           // Ensure instant buffer flushes
    g_setenv("WEBKIT_GST_AUDIO_RESAMPLING", "1", TRUE);     // Match audio server master clock

    // ====================================================================
    // CIAO Output System - MUST be initialized first
    // ====================================================================
    ciao_output_init(argc, argv);

    // ====================================================================
    // NEW: Configuration System (Milestone 2.0 Start)
    // ====================================================================
    const char *config_path = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--config") == 0 && i+1 < argc) {
            config_path = argv[i+1];
            break;
        }
    }

    config_init(config_path);

    // ====================================================================
    // CIAO Auto-Shutdown Feature
    // ====================================================================
    if (ciao_shutdown_seconds > 0) {
        ciao_debug("Scheduling auto-shutdown timer for %d seconds", ciao_shutdown_seconds);
        g_timeout_add_seconds(ciao_shutdown_seconds, on_auto_shutdown, NULL);
    }

    ciao_info("Starting %s %d.%d.%d", 
              config_get_project_name(), VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    gtk_init(&argc, &argv);

    // ====================================================================
    // WebKit + Persistence Layer (Single Point of Truth)
    // ====================================================================
    WebKitWebView *web_view = setup_webkit_webview();
    if (web_view == NULL) {
        ciao_error("Critical: Failed to initialize WebKit");
        return 1;
    }

    UrlBarUserAgent url_bar = {0};

    // ====================================================================
    // UI Layer - All window, menu, and layout construction
    // ====================================================================
    GtkWidget *window = create_main_window(web_view, &url_bar);
    if (window == NULL) {
        ciao_error("Critical: Failed to create main window");
        return 1;
    }

    ciao_info("%s started successfully with persistent cookies and User-Agent settings", 
              config_get_project_name());

    gtk_widget_show_all(window);
    gtk_main();

    ciao_info("%s terminated normally", config_get_project_name());
    return 0;
}