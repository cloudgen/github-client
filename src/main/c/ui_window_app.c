// =========================================================================
// CIAO DEFENSIVE CODING STYLE - UI WINDOW IMPLEMENTATION (GMAIL SPECIFIC)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + Strong Protection: 2026-04-18
// =========================================================================

#include "config.h"          // ← Add this
#include "ui_window_app.h"
#include "ui_menu_app.h"
#include "output.h"
#include "load_page_login_check.h"
#include "project.h"

// External declaration — assuming these are defined in a header or elsewhere
extern void inject_auto_follow_script(WebKitWebView *web_view);



// ──────────────────────────────────────────────
// create_main_window() - General Purpose Requirement: Create Main Application Window
// ──────────────────────────────────────────────
//
// Purpose:
//   Creates the main window, sets up layout (menu → url bar → ua label → webview),
//   connects signals, and performs initial page load.
//   This is the **Single Point of Truth** for UI layout in this Github client.
//
// Parameters:
//   Input:
//     - web_view     : WebKitWebView* - The initialized web view (must not be NULL)
//     - out_url_bar  : UrlBarUserAgent* - Output struct to receive URL bar components
//   Output/Return:
//     - GtkWidget* - The fully constructed main window (or NULL on failure)
//
// Dependencies:
//   - Functions called: gtk_window_new(), gtk_window_set_title(), gtk_window_set_default_size(),
//     g_signal_connect(), load_user_agent_from_settings(), add_url_bar_user_agent(),
//     build_main_menu_bar(), gtk_box_new(), gtk_box_pack_start(), gtk_container_add(),
//     load_page_with_login_check(), ciao_error(), ciao_info(), ciao_warn(), ciao_debug()
//   - Headers required: gtk/gtk.h, ui_window_app.h, ui_menu_app.h, url_bar_user_agent.h,
//     settings.h, load_page_login_check.h, project.h, output.h
//   - External: Relies on ciao_bookmark_index from output system
//
// CIAO Protection Rule (Sacred - Do Not Violate):
//   This function is the **Single Point of Truth** for UI layout in this Github client.
// 
//   Future AI assistants or developers **MUST NOT**:
//     - Remove any defensive NULL checks
//     - Inline this function into main.c
//     - Change the vertical packing order without strong justification
//     - Rename this function
//     - Move layout logic into other files
// 
//   Reason: UI layout is highly project-specific. Keeping it isolated makes
//   forking to other services (YouTube, etc.) much safer.
// 
//   This follows CIAO Principle 5 (Single Point of Entry) + Principle 18
//   (Protect Against Future Modification).
//
// Why This Design:
//   Centralized UI construction with clear vertical packing order and
//   proper initialization sequence (URL bar before initial page load).
//
// CIAO Principles Applied:
//   - Caution: Early NULL check and defensive window creation
//   - Intentional: Single Point of Truth for main window layout
//   - Anti-fragile: Graceful failure handling with detailed logging
//   - Over-protect: Preserves project-specific UI structure
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
GtkWidget* create_main_window(WebKitWebView *web_view, UrlBarUserAgent *out_url_bar)
{
    if (web_view == NULL) {
        ciao_error("create_main_window: web_view is NULL");
        return NULL;
    }

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if (window == NULL) {
        ciao_error("Failed to create main window");
        return NULL;
    }

    gtk_window_set_title(GTK_WINDOW(window), config_get_window_title());
    gtk_window_set_default_size(GTK_WINDOW(window), 
                                config_get_default_width(), 
                                config_get_default_height());

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create URL bar + UA label with correct persisted User-Agent
    char loaded_ua[1024] = {0};
    load_user_agent_from_settings(loaded_ua, sizeof(loaded_ua));
    const char *effective_ua = (loaded_ua[0] != '\0') ? loaded_ua : DEFAULT_USER_AGENT;

    *out_url_bar = add_url_bar_user_agent(web_view, effective_ua);

    // Build menu bar (Github specific)
    GtkWidget *menu_bar = build_main_menu_bar(out_url_bar->url_entry,
                                              out_url_bar->ua_label,
                                              web_view,
                                              GTK_WINDOW(window));

    // Main vertical layout
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    // Respect [ui] section from config.conf
    if (config_get_show_navigation_buttons()) {
        gtk_box_pack_start(GTK_BOX(vbox), out_url_bar->url_box, FALSE, FALSE, 0);
    }

    if (config_get_show_user_agent_label()) {
        gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(out_url_bar->ua_label), FALSE, FALSE, 2);
    }

    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(web_view), TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    // ====================================================================
    // Initial Page Load - Respect BOOKMARK / --bookmark
    // ====================================================================
    const char *initial_url = START_PAGE;

    if (ciao_bookmark_index > 0) {
        const char *bookmark_urls[] = {
            START_PAGE,      // index 0
            NULL
        };

        if (ciao_bookmark_index < 3 && bookmark_urls[ciao_bookmark_index]) {
            initial_url = bookmark_urls[ciao_bookmark_index];
            ciao_info("Started with bookmark index %d → %s", 
                      ciao_bookmark_index, initial_url);
        } else {
            ciao_warn("Bookmark index %d out of range, falling back to Start Page", 
                      ciao_bookmark_index);
        }
    } else {
        ciao_debug("Using default Start Page (no bookmark requested)");
    }

    load_page_with_login_check(web_view, initial_url);

    ciao_debug("Main window and layout created successfully (Github specific)");
    return window;
}