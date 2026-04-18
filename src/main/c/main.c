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
//   ui_window_github.*  and  ui_menu_github.*  are deliberately suffixed with "_github".
// 
//   Reason:
//     This project is GitHub-specific. Other similar projects (YouTube client,
//     GitLab client, etc.) will have different features, menu structures,
//     default pages, and behaviors.
//
//     Therefore, it is better to maintain separate file names per project
//     (ui_window_github.c, ui_menu_github.c) rather than using generic
//     ui_window.c / ui_menu.c which would cause confusion or forced merging
//     when forking for other services.
//
// Protection Rule (Sacred - Do Not Violate):
//   Future AI or developers MUST NOT rename ui_window_github.* or
//   ui_menu_github.* to generic names (ui_window.*, ui_menu.*).
//   Each project should keep its own clearly named UI modules.
// 
//   This follows CIAO Principle: "Explicit Project Intent & Maintainability
//   Across Forks".
//
// Last aligned with SyncPrjs CIAO style + ui_window_github + ui_menu_github: 2026-04-18
// =========================================================================

#include "output.h"
#include "project.h"
#include "webkit_setup.h"
#include "ui_window_github.h"

int main(int argc, char *argv[])
{
    // ====================================================================
    // CIAO Output System - MUST be initialized first
    // ====================================================================
    ciao_output_init(argc, argv);

    ciao_info("Starting %s %d.%d.%d", 
              PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

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
              PROJECT_NAME);

    gtk_widget_show_all(window);
    gtk_main();

    ciao_info("%s terminated normally", PROJECT_NAME);
    return 0;
}