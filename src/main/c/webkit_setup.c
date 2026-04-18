// =========================================================================
// CIAO DEFENSIVE CODING STYLE - WEBKIT SETUP IMPLEMENTATION
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System + Settings: 2026-04-18
// =========================================================================

#include "webkit_setup.h"
#include "output.h"
#include "project.h"
#include "cookies.h"
#include "settings.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// =========================================================================
// General Purpose Requirement: Create and Configure WebKitWebView
// 
// Purpose:
//   Creates WebKitWebContext → adds persistent cookies → loads persisted
//   User-Agent from settings.sqlite → creates WebKitWebView with correct UA.
// 
// Protection Rule:
//   This is the ONLY place in the entire project where WebKitWebContext
//   and WebKitWebView are created. Do not duplicate this logic.
// =========================================================================
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

    // First run: persist default UA
    if (loaded_ua[0] == '\0') {
        save_user_agent_to_settings(DEFAULT_USER_AGENT);
        ciao_info("First run: saved default User-Agent to settings.sqlite");
    }

    // ====================================================================
    // 4. Create WebKitWebView with context
    // ====================================================================
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new_with_context(web_context));
    if (web_view == NULL) {
        ciao_error("Failed to create WebKitWebView");
        g_object_unref(web_context);
        return NULL;
    }

    // ====================================================================
    // 5. Apply User-Agent to WebView
    // ====================================================================
    WebKitSettings *wk_settings = webkit_web_view_get_settings(web_view);
    if (wk_settings != NULL) {
        webkit_settings_set_user_agent(wk_settings, effective_ua);
        ciao_info("Applied User-Agent: %s", effective_ua);
    } else {
        ciao_warn("Failed to get WebKitSettings - UA may not be applied");
    }

    ciao_info("WebKitWebView fully initialized with persistent storage and User-Agent");
    return web_view;
}