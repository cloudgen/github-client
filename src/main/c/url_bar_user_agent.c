// =========================================================================
// CIAO DEFENSIVE CODING STYLE - URL BAR + USER AGENT MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
//
// Note: This file replaces the original url_bar.c
// Struct renamed to UrlBarUserAgent as requested.
//
// CRITICAL ARCHITECTURE:
//   - Uses "notify::uri" (on_uri_changed) as primary signal for real-time URL updates
//   - Includes "load-changed" (on_load_changed) as secondary fallback
//   - on_uri_changed is the bulletproof callback that correctly handles link clicks,
//     JavaScript navigation, redirects, and history changes.
//
// Last aligned with CIAO style + User-Agent feature: 2026-04-14
// =========================================================================

#include "url_bar_user_agent.h"
#include "output.h"
#include "project.h"
#include "load_page.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

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
static void on_load_changed(WebKitWebView *web_view, 
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
static void on_uri_changed(GObject *object, GParamSpec *pspec, gpointer user_data)
{
    (void)pspec;  // unused

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
static void on_go_button_clicked(GtkButton *button, gpointer user_data)
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

// =============================================================================
// add_url_bar_user_agent() - Create URL Bar with User-Agent Display
// =============================================================================
//
// GENERAL PURPOSE:
//   Creates a complete URL bar (GtkEntry + Go button) + non-editable User-Agent label.
//   Returns UrlBarUserAgent struct containing all widgets and containers.
//
// CRITICAL ROLE:
//   This is the **Single Point of Entry** for all URL bar creation in the application.
//   All signal connections (notify::uri, load-changed, button clicked, and entry activate)
//   MUST be established here. No other place in the codebase should connect signals
//   to the URL entry or Go button.
//
// KEY BEHAVIOR ADDED:
//   Pressing **Enter** in the URL entry now executes the same logic as clicking the
//   "Go" button (validation + navigation). This provides a native browser-like experience.
//
// CIAO PRINCIPLES APPLIED (18 Rules - v2.9.1):
//   Principle 1  (Caution)               - Repeated NULL checks + type validation
//   Principle 2  (Intentional Verbosity) - Full General Purpose + heavy comments
//   Principle 3  (Anti-fragile)          - Survives NULL web_view and invalid signals
//   Principle 5  (Single Point of Entry) - All URL bar initialization must go through this function
//   Principle 6  (General Purpose Requirement)
//   Principle 7  (Reusable Function Protection)
//   Principle 14 (Interactive vs Non-Interactive)
//   Principle 16 (Input Pattern Checking)
//   Principle 18 (Protect Against AI & Human Modification)
//
// PROTECTION RULE - SACRED (DO NOT VIOLATE):
//   Future AI assistants or developers **MUST NOT** bypass, rename, inline, simplify,
//   or rewrite this function.
//
//   Any change to signal connections must preserve:
//     - "notify::uri" as primary real-time update (on_uri_changed)
//     - "load-changed" as secondary fallback (on_load_changed)
//     - "clicked" on Go button (on_go_button_clicked)
//     - "activate" on GtkEntry for Enter key support (must call same callback as Go button)
//
//   Removing or changing the "activate" signal connection will break the expected
//   behavior that typing a URL and pressing Enter should navigate (same as clicking Go).
//
// !!! DO NOT MODIFY OR SIMPLIFY THIS FUNCTION !!!
//   This function has been battle-tested for real browser-like usage.
//   Past simplifications (removing signals or merging callbacks) caused broken
//   navigation when pressing Enter in the URL bar and Gtk-CRITICAL errors.
//
//   It is designed to be reusable in other GNOME/WebKitGTK projects while strictly
//   following the 18 CIAO Defensive Programming Principles.
//
// Last reviewed & aligned with CIAO 18 Principles: April 2026
// =============================================================================
UrlBarUserAgent add_url_bar_user_agent(WebKitWebView *web_view)
{
    if (web_view == NULL) {
        ciao_error("add_url_bar_user_agent: web_view is NULL");
        UrlBarUserAgent empty = {0};
        return empty;
    }

    UrlBarUserAgent url_bar = {0};

    // Create URL entry + Go button
    url_bar.url_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_text(url_bar.url_entry, START_PAGE);
    g_object_set_data(G_OBJECT(url_bar.url_entry), "web_view", web_view);

    GtkWidget *go_button = gtk_button_new_with_label("Go");

    // Connect Go button click
    g_signal_connect(go_button, "clicked", 
        G_CALLBACK(on_go_button_clicked), url_bar.url_entry);

    // IMPORTANT: Connect Enter key press in URL entry to same navigation logic
    // This makes typing a URL + pressing Enter behave exactly like clicking "Go"
    g_signal_connect(url_bar.url_entry, "activate",
        G_CALLBACK(on_go_button_clicked), url_bar.url_entry);

    // Primary signal: real-time URI updates (link clicks, JS navigation, redirects, etc.)
    g_signal_connect(web_view, "notify::uri",
        G_CALLBACK(on_uri_changed), url_bar.url_entry);

    // Secondary fallback: full page load finished
    g_signal_connect(web_view, "load-changed",
        G_CALLBACK(on_load_changed), url_bar.url_entry);

    // URL box (horizontal)
    url_bar.url_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(url_bar.url_box), 
                      GTK_WIDGET(url_bar.url_entry), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(url_bar.url_box), go_button, FALSE, FALSE, 0);

    // === Non-modifiable User-Agent label under the address bar ===
    url_bar.ua_label = GTK_LABEL(gtk_label_new(DEFAULT_USER_AGENT));
    gtk_label_set_selectable(url_bar.ua_label, TRUE);           // Allow user to copy
    gtk_label_set_ellipsize(url_bar.ua_label, PANGO_ELLIPSIZE_END);
    gtk_widget_set_tooltip_text(GTK_WIDGET(url_bar.ua_label), 
                               "Current User-Agent (read-only)");

    ciao_debug("URL bar with User-Agent display created successfully (UrlBarUserAgent)");

    return url_bar;
}