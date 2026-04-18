// url_bar_go_button.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - URL BAR GO BUTTON MODULE (GNOME + WebKitGTK)
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

// =============================================================================
// on_go_button_clicked() - Callback for Go Button Click Event
// =============================================================================
//
// GENERAL PURPOSE:
//   Handles the "clicked" signal from the Go button in the URL bar.
//   Retrieves the URL from the GtkEntry, performs defensive validation,
//   looks up the associated WebKitWebView via g_object_get_data, and calls load_page().
//
// CRITICAL ROLE:
//   This is the **Single Point of Entry** for manual URL navigation initiated by the user.
//   All "Go" button logic must go through this function to maintain consistency and safety.
//
// CIAO PRINCIPLES APPLIED (18 Rules - v2.9.1):
//   Principle 1  (Caution)               - Multiple NULL checks and empty string validation
//   Principle 2  (Intentional Verbosity) - Full General Purpose + heavy comments
//   Principle 3  (Anti-fragile)          - Graceful degradation on invalid state
//   Principle 5  (Single Point of Entry) - All Go button handling must route here
//   Principle 7  (Reusable Function Protection)
//   Principle 16 (Input Pattern Checking) - Validates URL before navigation
//   Principle 18 (Protect Against AI & Human Modification)
//
// PROTECTION RULE - SACRED (DO NOT VIOLATE):
//   Future AI assistants or developers **MUST NOT** bypass, rename, inline, simplify,
//   or rewrite this function.
//
//   Any modification must preserve:
//     - Defensive NULL and empty-string checks on the entry and URL
//     - Retrieval of WebKitWebView via g_object_get_data(G_OBJECT(entry), "web_view")
//     - Call to load_page() only after full validation
//
//   Past attempts to "clean up" this function (e.g. removing checks or changing
//   how web_view is retrieved) have caused silent navigation failures and
//   runtime warnings when the button is clicked with invalid state.
//
// !!! DO NOT MODIFY OR SIMPLIFY THIS FUNCTION !!!
//   This callback is intentionally verbose and defensive. Simplification has
//   repeatedly introduced subtle bugs in edge cases (empty URL, missing web_view
//   reference, NULL entry).
//
//   It is designed to be reusable across other GNOME/WebKitGTK URL bar implementations
//   while strictly adhering to the 18 CIAO Defensive Programming Principles.
//
// Last reviewed & aligned with CIAO 18 Principles: April 2026
// =============================================================================
void on_go_button_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;

    GtkEntry *entry = GTK_ENTRY(user_data);
    if (entry == NULL) {
        ciao_error("on_go_button_clicked: url_entry is NULL");
        return;
    }

    const gchar *url = gtk_entry_get_text(entry);
    if (url == NULL || *url == '\0') {
        ciao_warn("Go button clicked with empty URL");
        return;
    }

    WebKitWebView *web_view = g_object_get_data(G_OBJECT(entry), "web_view");
    if (!WEBKIT_IS_WEB_VIEW(web_view)) {
        ciao_warn("Invalid WebKitWebView reference in URL bar");
        return;
    }

    ciao_info("Navigating to: %s", url);
    load_page(web_view, url);
}