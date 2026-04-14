// =========================================================================
// CIAO DEFENSIVE CODING STYLE - MAIN APPLICATION ENTRY POINT (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// !!! MUST CALL ciao_output_init() VERY EARLY !!!
//
// Updated to use UrlBarUserAgent and User-Agent spoofing menu.
// Last aligned with SyncPrjs CIAO style + User-Agent feature: 2026-04-12
// =========================================================================

#include "output.h"                    // MUST be first - declares ciao_output_init()
#include "project.h"
#include "cookies.h"
#include "dialog_version.h"
#include "load_page.h"
#include "menu_exit.h"
#include "url_bar_user_agent.h"        // Updated: renamed + extended with UA support
#include "menu_bookmark_github.h"
#include "menu_user_agent.h"           // New: User Agent menu

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// =========================================================================
// General Purpose Requirement: Application Main Entry Point
// 
// Purpose:
//   Initializes the full GNOME/GTK + WebKitGTK application with
//   proper output system, cookie persistence, URL bar with User-Agent display,
//   and menus including User Agent spoofing.
// 
// Protection Rule:
//   Future AI must NOT move ciao_output_init() or remove defensive checks.
//   This is the Single Point of Application Startup.
// =========================================================================
int main(int argc, char *argv[])
{
    // ====================================================================
    // CIAO Output System MUST be initialized FIRST
    // ====================================================================
    ciao_output_init(argc, argv);

    ciao_info("Starting %s %d.%d.%d", 
              PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    gtk_init(&argc, &argv);

    // Create main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if (window == NULL) {
        ciao_error("Failed to create main window");
        return 1;
    }

    gtk_window_set_title(GTK_WINDOW(window), PROJECT_NAME);
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    ciao_debug("Main window created successfully");

    // ====================================================================
    // WebKit Context + Persistent Cookies
    // ====================================================================
    WebKitWebContext *web_context = webkit_web_context_new();
    if (web_context == NULL) {
        ciao_error("Failed to create WebKitWebContext");
        return 1;
    }

    add_cookie(web_context);
    ciao_debug("WebKitWebContext with persistent cookies initialized");

    // Create WebView
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new_with_context(web_context));
    if (web_view == NULL) {
        ciao_error("Failed to create WebKitWebView");
        return 1;
    }

    // ====================================================================
    // URL Bar + User-Agent Display (Updated)
    // ====================================================================
    UrlBarUserAgent url_bar = add_url_bar_user_agent(web_view);

    // ====================================================================
    // Menu Bar Setup
    // ====================================================================
    GtkWidget *menu_bar = gtk_menu_bar_new();

    // File Menu
    GtkWidget *file_menu = gtk_menu_new();

    // Bookmark menu (existing)
    GtkWidget *bookmark_menu_item = add_bookmark_menu_github(url_bar.url_entry);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), bookmark_menu_item);

    // New: User Agent menu
    GtkWidget *user_agent_menu_item = add_user_agent_menu(url_bar.ua_label, web_view);
    if (user_agent_menu_item != NULL) {
        gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), user_agent_menu_item);
    } else {
        ciao_warn("Failed to create User Agent menu");
    }

    // Exit menu item
    GtkWidget *exit_menu_item = gtk_menu_item_new_with_label("Exit");
    g_signal_connect(exit_menu_item, "activate", 
                    G_CALLBACK(on_exit_menu_item_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), exit_menu_item);

    GtkWidget *file_menu_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);

    // About Menu (existing)
    GtkWidget *about_menu = gtk_menu_new();
    GtkWidget *show_version_menu_item = gtk_menu_item_new_with_label("Show Version");
    g_signal_connect(show_version_menu_item, "activate", 
                    G_CALLBACK(on_show_version_activate), window);

    gtk_menu_shell_append(GTK_MENU_SHELL(about_menu), show_version_menu_item);

    GtkWidget *about_menu_item = gtk_menu_item_new_with_label("About");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(about_menu_item), about_menu);

    // Assemble menu bar
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), about_menu_item);

    // ====================================================================
    // Main Layout: URL bar + User-Agent label stacked vertically
    // ====================================================================
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Menu bar
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    // URL bar (entry + Go button)
    gtk_box_pack_start(GTK_BOX(vbox), url_bar.url_box, FALSE, FALSE, 0);

    // User-Agent display (non-modifiable label) - directly under address bar
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(url_bar.ua_label), FALSE, FALSE, 2);

    // WebView takes the remaining space
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(web_view), TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Initial page load
    load_page(web_view, START_PAGE);

    ciao_info("%s started successfully with User-Agent spoofing enabled", PROJECT_NAME);

    gtk_widget_show_all(window);
    gtk_main();

    ciao_info("%s terminated normally", PROJECT_NAME);
    return 0;
}