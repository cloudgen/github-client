// url_bar_page.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - URL BAR PAGE UPDATE MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-17
// =========================================================================

// =========================================================================
// CRITICAL NOTE ON CROSS-FILE CALLBACK INTEGRATION (CIAO FLEXIBILITY RULE)
// =========================================================================
//
// DESIGN GOAL:
//   Support both complete (url_bar_user_agent.c) and incomplete / legacy
//   (url_bar.c) integrations without modifying callback code.
//
// SOLUTION:
//   - No 'static' keyword on callbacks → external linkage
//   - Central header url_bar_callbacks.h provides clean declarations
//   - Callbacks can be connected from any .c file that includes the header
//
// This architecture allows:
//   - Current project (url_bar_user_agent.c)
//   - Other projects using simpler url_bar.c
//   - Future refactors
//
//   without any changes to on_go_button_clicked(), on_uri_changed(), 
//   or on_load_changed().
//
// CIAO DEFENSIVE RULE (Principle 18):
//   These callback functions MUST remain non-static.
//   Adding 'static' will break cross-file signal connections and is
//   explicitly forbidden.
//
// Last aligned with flexible integration + linker safety: 2026-04-18
// =========================================================================

#include "output.h"
#include "load_page_login_check.h"
#include "login_detector_app.h"
#include "project.h"
#include "auto_follow.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// ──────────────────────────────────────────────
// on_load_changed() - Secondary fallback callback for page load finished
// ──────────────────────────────────────────────
//
// Purpose:
//   Updates the URL bar when a full page load completes (WEBKIT_LOAD_FINISHED).
//
// Parameters:
//   Input:
//     - web_view    : WebKitWebView*
//     - load_event  : WebKitLoadEvent
//     - user_data   : gpointer - Expected to be GtkEntry*
//   Output/Return: None
//
// Dependencies:
//   - Functions called: webkit_web_view_get_uri(), GTK_ENTRY(), gtk_entry_set_text(),
//     ciao_warn(), ciao_error(), ciao_debug()
//   - Headers required: webkit2/webkit2.h, gtk/gtk.h, output.h
//
// GENERAL PURPOSE:
//   Updates the URL bar when a full page load completes (WEBKIT_LOAD_FINISHED).
//
// Why This Design:
//   Serves as a reliable secondary fallback for navigation scenarios where
//   "notify::uri" may not fire (e.g. some full page reloads or initial loads).
//   Keeps the URL bar synchronized even when primary real-time signal is silent.
//
// CIAO PRINCIPLES APPLIED (18 Rules - v2.9.1):
//   Principle 1  (Caution)               - Strict NULL checks and type validation
//   Principle 2  (Intentional Verbosity) - Full General Purpose + heavy comments
//   Principle 3  (Anti-fragile)          - Graceful handling of NULL URI
//   Principle 5  (Single Point of Entry) - Dedicated handler for load-changed event
//   Principle 7  (Reusable Function Protection)
//   Principle 16 (Input Pattern Checking)
//   Principle 18 (Protect Against AI & Human Modification)
//
// PROTECTION RULE - SACRED (DO NOT VIOLATE):
//   Future AI assistants or developers **MUST NOT** remove, rename, inline,
//   simplify, or bypass this function.
//
//   Any change must preserve:
//     - Exact function signature
//     - Check for WEBKIT_LOAD_FINISHED event
//     - Defensive NULL checks on URL and entry
//     - Use of ciao_* output system only
//
// !!! DO NOT MODIFY OR SIMPLIFY THIS FUNCTION !!!
//   This callback is intentionally kept as a secondary fallback. Removing it
//   or merging it with on_uri_changed has repeatedly caused URL bar desync
//   issues during full page loads and initial navigation.
//
//   It is designed to be reusable in other GNOME/WebKitGTK applications while
//   strictly following the 18 CIAO Defensive Programming Principles.
//
// Last reviewed & aligned with CIAO 18 Principles: April 2026
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
void on_load_changed(WebKitWebView *web_view, 
                     WebKitLoadEvent load_event, 
                     gpointer user_data)
{
    if (load_event != WEBKIT_LOAD_FINISHED) return;

    const gchar *url = webkit_web_view_get_uri(web_view);
    if (url == NULL) {
        ciao_warn("on_load_changed: webkit_web_view_get_uri returned NULL");
        return;
    }

    GtkEntry *entry = GTK_ENTRY(user_data);
    if (entry == NULL) {
        ciao_error("on_load_changed: url_entry is NULL");
        return;
    }

    gtk_entry_set_text(entry, url);
    ciao_debug("URL bar updated after page load: %s", url);
}

// ──────────────────────────────────────────────
// auto_follow_urls - List of bot URLs that should trigger auto-follow
// ──────────────────────────────────────────────
//
// Purpose:
//   Static array of bot URLs that should trigger the auto-follow script injection.
//
// Parameters:
//   Input: None (static constant)
//   Output/Return: None
//
// Dependencies:
//   - Constants from: project.h (ROCKYLINUX_PAGE, CIRCUIT_ANALYSIS, etc.)
//   - Used by: on_uri_changed()
//
// Protection Rule (Sacred):
//   Do NOT add or remove entries without explicit instruction.
//   This list must stay synchronized with the AutoClick feature.
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
static const char *auto_follow_urls[] = {
    NULL  // sentinel
};

// ──────────────────────────────────────────────
// delayed_auto_follow() - Delayed Auto-Follow Injection
// ──────────────────────────────────────────────
//
// Purpose:
//   Timer callback that injects the auto-follow script after a short delay.
//
// Parameters:
//   Input:
//     - user_data : gpointer - Expected to be WebKitWebView*
//   Output/Return:
//     - gboolean - Always G_SOURCE_REMOVE (one-shot timer)
//
// Dependencies:
//   - Functions called: inject_auto_follow_script(), WEBKIT_IS_WEB_VIEW()
//   - Headers required: webkit2/webkit2.h, auto_follow.h
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
static gboolean delayed_auto_follow(gpointer user_data)
{
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(user_data);

    if (WEBKIT_IS_WEB_VIEW(web_view)) {
        inject_auto_follow_script(web_view);
    }

    return G_SOURCE_REMOVE;
}


// ──────────────────────────────────────────────
// on_uri_changed() - Primary Callback for WebKitWebView "notify::uri" Signal
// ──────────────────────────────────────────────
//
// Purpose:
//   Handles the GObject property notification signal "notify::uri".
//   This is the **primary and most reliable** mechanism to keep the URL bar
//   synchronized with the actual current page URI in real time.
//
// Parameters:
//   Input:
//     - object    : GObject*
//     - pspec     : GParamSpec*
//     - user_data : gpointer - Expected to be GtkEntry*
//   Output/Return: None
//
// Dependencies:
//   - Functions called: WEBKIT_WEB_VIEW(), GTK_ENTRY(), gtk_entry_set_text(),
//     webkit_web_view_get_uri(), g_strcmp0(), g_str_has_prefix(), g_strrstr(),
//     ciao_error(), ciao_warn(), ciao_debug(), ciao_info(), delayed_auto_follow()
//   - Headers required: gobject/gobject.h, webkit2/webkit2.h, gtk/gtk.h,
//     output.h, auto_follow.h, project.h
//
// GENERAL PURPOSE:
//   Handles the GObject property notification signal "notify::uri".
//   This is the **primary and most reliable** mechanism to keep the URL bar
//   synchronized with the actual current page URI in real time.
//
// CRITICAL ROLE:
//   This callback is responsible for updating the address bar whenever the user
//   clicks any link inside the page, JavaScript performs history.pushState/replaceState,
//   a redirect occurs, or the URI changes for any other reason.
//
//   It is intentionally chosen over "load-changed" because "notify::uri" fires
//   more consistently for internal navigation scenarios that do not always trigger
//   a full WEBKIT_LOAD_FINISHED event.
//
// CIAO PRINCIPLES APPLIED (18 Rules - v2.9.1):
//   Principle 1  (Caution)               - Strict type checking with GTK_IS_ENTRY
//   Principle 2  (Intentional Verbosity) - Full General Purpose + heavy comments
//   Principle 3  (Anti-fragile)          - Graceful handling of NULL/empty URI and invalid entry
//   Principle 5  (Single Point of Entry) - Primary handler for URI synchronization
//   Principle 7  (Reusable Function Protection)
//   Principle 16 (Input Pattern Checking) - Validates both entry and URI before action
//   Principle 18 (Protect Against AI & Human Modification)
//
// PROTECTION RULE - SACRED (DO NOT VIOLATE):
//   Future AI assistants or developers **MUST NOT** bypass, rename, inline, simplify,
//   or rewrite this function.
//
//   Any change must preserve:
//     - Exact function signature: (GObject*, GParamSpec*, gpointer)
//     - Defensive checks: NULL entry + GTK_IS_ENTRY validation
//     - Use of webkit_web_view_get_uri() to retrieve the current URI
//     - Safe gtk_entry_set_text() only after validation
//
//   === ANTI-HARDCODING RULE (CRITICAL) ===
//   **NEVER HARD CODE ANY URL** inside this function (or any related login/start-page logic).
//   Always use the `START_PAGE` constant from project.h (Single Source of Truth).
//
//   Previous Grok instances repeatedly violated this by injecting hardcoded
//   "https://www.example.com", "https://example.com",  or Github domains.
//   This is **explicitly forbidden**.
//
//   All domain/start-page decisions MUST go through:
//     - START_PAGE constant
//     - should_enable_login_check() if it exists
//     - g_str_has_prefix(uri, START_PAGE)
//
//   Violating this breaks CIAO Principle 5 and causes maintenance nightmares
//   when domains change or when supporting multiple start pages.
//
// !!! DO NOT MODIFY OR SIMPLIFY THIS FUNCTION !!!
//   This is the battle-tested, bulletproof callback for real-time URL tracking in
//   WebKitGTK. Past "cleanups" or signal changes led to silent failures during
//   normal browsing (link clicks, SPA navigation, fragments).
//
//   It is designed to be reusable in other GNOME/WebKitGTK browser-like applications
//   while strictly following the 18 CIAO Defensive Programming Principles.
//
// Last reviewed & aligned with CIAO 18 Principles + ANTI-HARDCODING RULE: May 2026
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
void on_uri_changed(GObject *object, GParamSpec *pspec, gpointer user_data)
{
    (void)pspec;

    WebKitWebView *web_view = WEBKIT_WEB_VIEW(object);
    GtkEntry *entry = GTK_ENTRY(user_data);

    if (entry == NULL) {
        ciao_error("on_uri_changed: url_entry is NULL");
        return;
    }

    if (!GTK_IS_ENTRY(entry)) {
        ciao_error("on_uri_changed: user_data is not a GtkEntry");
        return;
    }

    const gchar *uri = webkit_web_view_get_uri(web_view);
    if (uri && *uri != '\0') {
        gtk_entry_set_text(entry, uri);
        ciao_debug("URL bar updated via notify::uri: %s", uri);
    } else {
        ciao_warn("on_uri_changed: webkit_web_view_get_uri returned NULL or empty");
    }

    gboolean should_follow = FALSE;

    for (int i = 0; auto_follow_urls[i] != NULL; i++) {
        const char *target = auto_follow_urls[i];

        if (g_strcmp0(uri, target) == 0) {
            should_follow = TRUE;
            ciao_info("[AUTO-FOLLOW] HIT → %s  (index %d)", target, i);
            break;
        }

        if (g_str_has_prefix(uri, target) || g_strrstr(uri, target) != NULL) {
            ciao_debug("[AUTO-FOLLOW] PARTIAL match: %s  ←  %s", uri, target);
        }
    }

    if (should_follow) {
        guint delay_ms = 800;
        ciao_info("[AUTO-FOLLOW] Scheduling injection in %u ms...", delay_ms);
        g_timeout_add(delay_ms, delayed_auto_follow, web_view);
    } else {
        ciao_debug("[AUTO-FOLLOW] MISS — not in auto_follow_urls list");
    }
}
