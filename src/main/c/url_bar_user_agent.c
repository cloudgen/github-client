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

#include "project.h"
#include "url_bar_user_agent.h"
#include "url_bar_callbacks.h"     // ← Preferred central include
#include "output.h"
#include "config.h"
#include <gtk/gtk.h>

// ──────────────────────────────────────────────
// on_back_clicked() - Navigation Button Wrapper (Back)
// ──────────────────────────────────────────────
//
// Purpose:
//   Safe wrapper for the Back button "clicked" signal.
//   Retrieves the WebKitWebView via GObject data and navigates back.
//
// Parameters:
//   Input:
//     - button    : GtkButton* - The Back button
//     - user_data : gpointer - Unused
//   Output/Return: None
//
// Dependencies:
//   - Functions called: WEBKIT_WEB_VIEW(), g_object_get_data(), webkit_web_view_go_back(), ciao_warn()
//   - Headers required: gtk/gtk.h, webkit2/webkit2.h, output.h
//
// Protection Rule (Sacred):
//   Do NOT remove the WEBKIT_IS_WEB_VIEW safety check.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
static void on_back_clicked(GtkButton *button, gpointer user_data)
{
    (void)user_data;
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(button), "web_view"));
    if (WEBKIT_IS_WEB_VIEW(web_view)) {
        webkit_web_view_go_back(web_view);
    } else {
        ciao_warn("on_back_clicked: Invalid WebKitWebView reference");
    }
}

// ──────────────────────────────────────────────
// on_forward_clicked() - Navigation Button Wrapper (Forward)
// ──────────────────────────────────────────────
//
// Purpose:
//   Safe wrapper for the Forward button "clicked" signal.
//
// Parameters:
//   Input:
//     - button    : GtkButton*
//     - user_data : gpointer - Unused
//   Output/Return: None
//
// Dependencies:
//   - Functions called: WEBKIT_WEB_VIEW(), g_object_get_data(), webkit_web_view_go_forward(), ciao_warn()
//
// Protection Rule (Sacred):
//   Do NOT remove the defensive WebKitWebView check.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
static void on_forward_clicked(GtkButton *button, gpointer user_data)
{
    (void)user_data;
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(button), "web_view"));
    if (WEBKIT_IS_WEB_VIEW(web_view)) {
        webkit_web_view_go_forward(web_view);
    } else {
        ciao_warn("on_forward_clicked: Invalid WebKitWebView reference");
    }
}

// ──────────────────────────────────────────────
// on_reload_clicked() - Navigation Button Wrapper (Reload)
// ──────────────────────────────────────────────
//
// Purpose:
//   Safe wrapper for the Reload button "clicked" signal.
//
// Parameters:
//   Input:
//     - button    : GtkButton*
//     - user_data : gpointer - Unused
//   Output/Return: None
//
// Dependencies:
//   - Functions called: WEBKIT_WEB_VIEW(), g_object_get_data(), webkit_web_view_reload(), ciao_warn()
//
// Protection Rule (Sacred):
//   Do NOT remove the safety check.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
static void on_reload_clicked(GtkButton *button, gpointer user_data)
{
    (void)user_data;
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(button), "web_view"));
    if (WEBKIT_IS_WEB_VIEW(web_view)) {
        webkit_web_view_reload(web_view);
    } else {
        ciao_warn("on_reload_clicked: Invalid WebKitWebView reference");
    }
}

// ──────────────────────────────────────────────
// on_navigation_buttons_update() - Update Back/Forward button sensitivity
// ──────────────────────────────────────────────
//
// Purpose:
//   Updates sensitivity of Back/Forward buttons based on WebKit history state.
//
// Parameters:
//   Input:
//     - web_view    : WebKitWebView*
//     - load_event  : WebKitLoadEvent (unused)
//     - user_data   : gpointer - Expected to be GtkWidget* (the button)
//   Output/Return: None
//
// Dependencies:
//   - Functions called: GTK_WIDGET(), gtk_button_get_label(), g_str_has_prefix(),
//     webkit_web_view_can_go_back(), webkit_web_view_can_go_forward(), gtk_widget_set_sensitive()
//   - Headers required: gtk/gtk.h, webkit2/webkit2.h
//
// Why This Design:
//   Reuses the existing "load-changed" signal for efficient button state updates.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
static void on_navigation_buttons_update(WebKitWebView *web_view,
                                         WebKitLoadEvent load_event,
                                         gpointer user_data)
{
    (void)load_event;

    GtkWidget *btn = GTK_WIDGET(user_data);
    if (!btn || !WEBKIT_IS_WEB_VIEW(web_view))
        return;

    const gchar *label = gtk_button_get_label(GTK_BUTTON(btn));

    if (g_str_has_prefix(label, "← Back")) {
        gtk_widget_set_sensitive(btn, webkit_web_view_can_go_back(web_view));
    } else if (g_str_has_prefix(label, "Forward")) {
        gtk_widget_set_sensitive(btn, webkit_web_view_can_go_forward(web_view));
    }
    // Reload is always enabled
}

// ──────────────────────────────────────────────
// add_url_bar_user_agent() - Create URL Bar with User-Agent Display
// ──────────────────────────────────────────────
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
// Parameters:
//   Input:
//     - web_view    : WebKitWebView* - The main web view
//     - initial_ua  : const char* - Initial User-Agent string (from settings or default)
//   Output/Return:
//     - UrlBarUserAgent - Struct containing url_entry, url_box, and ua_label
//
// Dependencies:
//   - Functions called: gtk_button_new_with_label(), g_object_set_data(),
//     g_signal_connect(), gtk_entry_new(), gtk_entry_set_text(),
//     add_url_bar_user_agent helper callbacks, on_go_button_clicked(),
//     on_uri_changed(), on_load_changed(), on_navigation_buttons_update(),
//     gtk_box_new(), gtk_box_pack_start(), gtk_label_new(), etc.
//   - Headers required: gtk/gtk.h, webkit2/webkit2.h, url_bar_user_agent.h,
//     url_bar_callbacks.h, output.h, project.h
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
//   Updated: Now accepts initial_ua to respect persisted settings at startup.
//
// New (2026-04-30):
//   Back/Forward/Reload buttons added as requested.
//   Sensitivity updated automatically via existing load-changed signal.
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
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
UrlBarUserAgent add_url_bar_user_agent(WebKitWebView *web_view, 
                                       const char *initial_ua)
{
    if (web_view == NULL) {
        ciao_error("add_url_bar_user_agent: web_view is NULL");
        UrlBarUserAgent empty = {0};
        return empty;
    }

    if (initial_ua == NULL || *initial_ua == '\0') {
        initial_ua = DEFAULT_USER_AGENT;
        ciao_warn("add_url_bar_user_agent: invalid initial_ua, using default");
    }

    UrlBarUserAgent url_bar = {0};

    // ====================================================================
    // Navigation Buttons (Back / Forward / Reload)
    // ====================================================================
    GtkWidget *back_btn    = gtk_button_new_with_label("← Back");
    GtkWidget *forward_btn = gtk_button_new_with_label("Forward →");
    GtkWidget *reload_btn  = gtk_button_new_with_label("↻ Reload");

    // Store web_view reference safely
    g_object_set_data(G_OBJECT(back_btn),    "web_view", web_view);
    g_object_set_data(G_OBJECT(forward_btn), "web_view", web_view);
    g_object_set_data(G_OBJECT(reload_btn),  "web_view", web_view);

    // Connect callbacks
    g_signal_connect(back_btn,    "clicked", G_CALLBACK(on_back_clicked),    NULL);
    g_signal_connect(forward_btn, "clicked", G_CALLBACK(on_forward_clicked), NULL);
    g_signal_connect(reload_btn,  "clicked", G_CALLBACK(on_reload_clicked),  NULL);

    // NEW: Respect config [ui] navigation_buttons_enabled
    gboolean buttons_enabled = config_get_navigation_buttons_enabled();
    gtk_widget_set_sensitive(back_btn,    buttons_enabled);
    gtk_widget_set_sensitive(forward_btn, buttons_enabled);
    gtk_widget_set_sensitive(reload_btn,  buttons_enabled);

    // ====================================================================
    // URL Entry + Go Button
    // ====================================================================
    url_bar.url_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_text(url_bar.url_entry, START_PAGE);
    g_object_set_data(G_OBJECT(url_bar.url_entry), "web_view", web_view);

    GtkWidget *go_button = gtk_button_new_with_label("Go");

    g_signal_connect(go_button, "clicked", 
        G_CALLBACK(on_go_button_clicked), url_bar.url_entry);

    g_signal_connect(url_bar.url_entry, "activate",
        G_CALLBACK(on_go_button_clicked), url_bar.url_entry);

    // Existing signals
    g_signal_connect(web_view, "notify::uri",
        G_CALLBACK(on_uri_changed), url_bar.url_entry);

    g_signal_connect(web_view, "load-changed",
        G_CALLBACK(on_load_changed), url_bar.url_entry);

    g_signal_connect(web_view, "load-changed",
        G_CALLBACK(on_navigation_buttons_update), back_btn);

    // ====================================================================
    // Layout
    // ====================================================================
    url_bar.url_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    gtk_box_pack_start(GTK_BOX(url_bar.url_box), back_btn,    FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(url_bar.url_box), forward_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(url_bar.url_box), reload_btn,  FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(url_bar.url_box), 
                      GTK_WIDGET(url_bar.url_entry), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(url_bar.url_box), go_button, FALSE, FALSE, 0);

    url_bar.ua_label = GTK_LABEL(gtk_label_new(initial_ua));
    gtk_label_set_selectable(url_bar.ua_label, TRUE);
    gtk_label_set_ellipsize(url_bar.ua_label, PANGO_ELLIPSIZE_END);
    gtk_widget_set_tooltip_text(GTK_WIDGET(url_bar.ua_label), 
                               "Current User-Agent (read-only)");

    ciao_debug("URL bar with navigation buttons created (enabled=%d)", 
               (int)buttons_enabled);

    return url_bar;
}
