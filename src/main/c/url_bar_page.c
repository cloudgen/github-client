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
#include "load_page.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// =========================================================================
// on_load_changed() - Secondary fallback callback for page load finished
// =========================================================================
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
// =========================================================================
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

// =============================================================================
// on_uri_changed() - Primary Callback for WebKitWebView "notify::uri" Signal
// =============================================================================
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
//   Replacing this callback with "load-changed" (or removing the GTK_IS_ENTRY check)
//   has repeatedly caused Gtk-CRITICAL assertions ("gtk_entry_get_text: assertion
//   'GTK_IS_ENTRY (entry)' failed") and broken URL bar updates on internal link clicks.
//
// !!! DO NOT MODIFY OR SIMPLIFY THIS FUNCTION !!!
//   This is the battle-tested, bulletproof callback for real-time URL tracking in
//   WebKitGTK. Past "cleanups" or signal changes led to silent failures during
//   normal browsing (link clicks, SPA navigation, fragments).
//
//   It is designed to be reusable in other GNOME/WebKitGTK browser-like applications
//   while strictly following the 18 CIAO Defensive Programming Principles.
//
// Last reviewed & aligned with CIAO 18 Principles: April 2026
// =============================================================================
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
}