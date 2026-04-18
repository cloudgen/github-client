// =========================================================================
// CIAO DEFENSIVE CODING STYLE - UI WINDOW IMPLEMENTATION (GITHUB SPECIFIC)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + Strong Protection: 2026-04-18
// =========================================================================

#include "ui_window_github.h"
#include "ui_menu_github.h"
#include "output.h"
#include "load_page.h"
#include "project.h"

// =========================================================================
// General Purpose Requirement: Create Main Application Window
// 
// Purpose:
//   Creates the main window, sets up layout (menu → url bar → ua label → webview),
//   connects signals, and performs initial page load.
//
// CIAO Protection Rule (Sacred - Do Not Violate):
//   This function is the **Single Point of Truth** for UI layout in this GitHub client.
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
// =========================================================================
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

    gtk_window_set_title(GTK_WINDOW(window), PROJECT_NAME);
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create URL bar + UA label with correct persisted User-Agent
    // We need to get the effective UA from webkit_setup or pass it down.
    // For now, we read it again (simple & safe)
    char loaded_ua[1024] = {0};
    load_user_agent_from_settings(loaded_ua, sizeof(loaded_ua));
    const char *effective_ua = (loaded_ua[0] != '\0') ? loaded_ua : DEFAULT_USER_AGENT;

    *out_url_bar = add_url_bar_user_agent(web_view, effective_ua);

    // Build menu bar (GitHub specific)
    GtkWidget *menu_bar = build_main_menu_bar(out_url_bar->url_entry,
                                              out_url_bar->ua_label,
                                              web_view,
                                              GTK_WINDOW(window));

    // Main vertical layout
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), out_url_bar->url_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(out_url_bar->ua_label), FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(web_view), TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Initial page load
    load_page(web_view, START_PAGE);

    ciao_debug("Main window and layout created successfully (GitHub specific)");
    return window;
}