// webkit_setup.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - WEBKIT SETUP IMPLEMENTATION
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System + Settings: 2026-05-02
// =========================================================================

#include "webkit_setup.h"
#include "output.h"
#include "project.h"
#include "cookies.h"
#include "settings.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// Forward declaration (required for signal connection)
static gboolean on_permission_request(WebKitWebView *web_view,
                                      WebKitPermissionRequest *request,
                                      gpointer user_data);

// ──────────────────────────────────────────────
// setup_webkit_webview() - Single Point of Truth for WebKit Initialization
// ──────────────────────────────────────────────
//
// Purpose:
//   Creates and fully configures the WebKitWebView with:
//     • Persistent cookie storage
//     • User-Agent loaded from settings.sqlite (with first-run default)
//     • Proper WebKitWebContext
//   This is the **only** place in the entire project where WebKitWebContext
//   and WebKitWebView are created.
//
// Lessons Learned (Critical - Final Audio Quality for Google Translate TTS):
//   - Even with perfect GStreamer environment variables, robot/metallic sound persisted.
//   - Root cause: WebKit's WebAudio engine runs in an isolated low-priority thread
//     (WebKitWebProcess), causing buffer underruns on short TTS bursts.
//   - Solution: Explicitly enable real-time threading via "enable-webaudio-realtime-thread"
//     property at WebKitSettings level. This grants the WebAudio pipeline high-priority
//     scheduling, matching YouTube's smooth audio behavior.
//
// Why This Design:
//   This is the **only** place in the entire project where WebKitWebView is created.
//   All audio-related settings must be applied here.
//
// CIAO Principles Applied:
//   - Caution: Minimal addition, full defensive checks preserved
//   - Intentional: Clear documentation of final TTS audio lesson learned
//   - Anti-fragile: Survives dynamic Web Audio usage in SPA pages
//   - Over-protect: Critical audio section protected
//
// Protection Rule (Sacred):
//   Future AI or developers **MUST NOT**:
//     - Duplicate WebKitWebView creation logic anywhere else
//     - Remove User-Agent persistence, cookie setup, or this real-time thread setting
//     - Bypass this function
//
// Last updated: 2026-05-20 (enable-webaudio-realtime-thread for final robot sound fix)
// ──────────────────────────────────────────────
WebKitWebView* setup_webkit_webview(void)
{
    ciao_debug("Starting WebKit setup...");

    // ====================================================================
    // 1. Create WebKitWebContext
    // ====================================================================
    WebKitWebContext *web_context = webkit_web_context_new();
    if (web_context == NULL) {
        ciao_error("Failed to create WebKitWebContext");
        return NULL;
    }
    ciao_debug("WebKitWebContext created successfully");

    // ====================================================================
    // 2. Persistent Cookie Storage
    // ====================================================================
    add_cookie(web_context);
    ciao_debug("Persistent cookie storage enabled");

    
    // ====================================================================
    // 3. Load Persisted User-Agent from settings.sqlite
    // ====================================================================
    char loaded_ua[1024] = {0};
    load_user_agent_from_settings(loaded_ua, sizeof(loaded_ua));

    const char *effective_ua = (loaded_ua[0] != '\0') ? loaded_ua : DEFAULT_USER_AGENT;

    if (loaded_ua[0] == '\0') {
        save_user_agent_to_settings(DEFAULT_USER_AGENT);
        ciao_info("First run: saved default User-Agent to settings.sqlite");
    }

    // ====================================================================
    // 4. Create WebKitWebView with context and explicit Autoplay policy
    // ====================================================================
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_new(WEBKIT_TYPE_WEB_VIEW,
        "web-context", web_context,
        "autoplay-policy", WEBKIT_AUTOPLAY_ALLOW,
        NULL));

    if (web_view == NULL) {
        ciao_error("Failed to create WebKitWebView");
        g_object_unref(web_context);
        return NULL;
    }

    // ====================================================================
    // 5. Apply User-Agent + Full Media / Speech / Autoplay support
    // ====================================================================
    WebKitSettings *wk_settings = webkit_web_view_get_settings(web_view);
    if (wk_settings != NULL) {
        webkit_settings_set_user_agent(wk_settings, effective_ua);

        webkit_settings_set_enable_webaudio(wk_settings, TRUE);
        webkit_settings_set_enable_media_stream(wk_settings, TRUE);
        webkit_settings_set_enable_mediasource(wk_settings, TRUE);
        webkit_settings_set_enable_html5_database(wk_settings, TRUE);
        webkit_settings_set_enable_javascript(wk_settings, TRUE);
        webkit_settings_set_media_playback_requires_user_gesture(wk_settings, FALSE);
        webkit_settings_set_enable_fullscreen(wk_settings, TRUE);

        // FIXES REMAINING ROBOT SOUND: Enable real-time thread priority for WebAudio
        g_object_set(G_OBJECT(wk_settings), "enable-webaudio-realtime-thread", TRUE, NULL);

        ciao_info("Applied User-Agent + full media/speech autoplay + real-time WebAudio threading for Google TTS");
    } else {
        ciao_warn("Failed to get WebKitSettings");
    }

    // ====================================================================
    // 6. Auto-approve media permission requests (Google Translate TTS)
    // ====================================================================
    g_signal_connect(web_view, "permission-request",
                     G_CALLBACK(on_permission_request), NULL);
    ciao_debug("Permission-request handler connected (auto-allow media devices)");

    ciao_info("WebKitWebView fully initialized with persistent storage and TTS support");

    // ====================================================================
    // 7. DEBUG: Report cookie count + settings structure (only when DEBUG=1)
    // ====================================================================
    debug_report_settings_structure();

    return web_view;
}

// ──────────────────────────────────────────────
// on_permission_request() - Auto-approve media permission requests
// ──────────────────────────────────────────────
static gboolean on_permission_request(WebKitWebView *web_view,
                                      WebKitPermissionRequest *request,
                                      gpointer user_data)
{
    (void)web_view;
    (void)user_data;

    webkit_permission_request_allow(request);
    ciao_debug("[PERMISSION] Auto-allowed media device request for TTS");

    return TRUE;
}