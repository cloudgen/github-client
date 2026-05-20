// =========================================================================
// CIAO DEFENSIVE CODING STYLE - UI MENU IMPLEMENTATION (GM SPECIFIC)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + Strong Protection: 2026-04-18
// =========================================================================

#include "ui_menu_app.h"
#include "config.h"
#include "menu_bookmark_app.h"
#include "menu_user_agent.h"
#include "menu_exit.h"
#include "dialog_version.h"
#include "output.h"

// ──────────────────────────────────────────────
// build_main_menu_bar() - General Purpose Requirement: Build Main Menu Bar
// ──────────────────────────────────────────────
//
// Purpose:
//   Creates and assembles the complete menu bar (File → Bookmark, User Agent,
//   Exit | About → Show Version).
//   This is the **Single Point of Truth** for menu construction in the Github client.
//
// Parameters:
//   Input:
//     - url_entry      : GtkEntry* - The main URL bar (required for bookmark and AutoClick)
//     - ua_label       : GtkLabel* - The User-Agent display label
//     - web_view       : WebKitWebView* - The main web view
//     - parent_window  : GtkWindow* - Parent window for dialogs
//   Output/Return:
//     - GtkWidget* - The fully constructed menu bar (or NULL on failure)
//
// Dependencies:
//   - Functions called: gtk_menu_bar_new(), gtk_menu_new(), gtk_menu_item_new_with_label(),
//     g_signal_connect(), add_bookmark_menu_app(), add_user_agent_menu(),
//     on_autoclick_activate(), on_exit_menu_item_activate(), on_show_version_activate(),
//     ciao_error(), ciao_warn(), ciao_debug()
//   - Headers required: gtk/gtk.h, ui_menu_app.h, menu_bookmark_app.h, menu_user_agent.h,
//     menu_exit.h, dialog_version.h, output.h
//   - External signals: Connects multiple "activate" signals
//
// CIAO Protection Rule (Sacred - Do Not Violate):
//   This is the **Single Point of Truth** for menu construction in the Github client.
//
// Why This Design:
//   Centralized menu assembly ensures consistent structure and easy maintenance
//   for Github-specific menu items (AutoClick, Bookmark, User Agent, etc.).
//
// CIAO Principles Applied:
//   - Caution: Early NULL parameter validation
//   - Intentional: Single Point of Truth for all menu construction
//   - Anti-fragile: Graceful handling of failed submenu creation
//   - Over-protect: Preserves Github-specific menu layout
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Rename, inline, or simplify this function
//     - Move menu building logic to main.c or ui_window_app.c
//     - Remove any menu item or signal connection
//   This function must remain the central menu builder.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
// ──────────────────────────────────────────────
// build_main_menu_bar() - Respect [bookmarks] enabled for both Bookmark and AutoClick
// ──────────────────────────────────────────────
GtkWidget* build_main_menu_bar(GtkEntry *url_entry,
                               GtkLabel *ua_label,
                               WebKitWebView *web_view,
                               GtkWindow *parent_window)
{
    if (!url_entry || !web_view || !parent_window) {
        ciao_error("build_main_menu_bar: invalid parameters (NULL pointer detected)");
        return NULL;
    }

    GtkWidget *menu_bar = gtk_menu_bar_new();
    GtkWidget *file_menu = gtk_menu_new();

    // AutoClick item — now respects bookmarks config
    if (config_get_bookmarks_enabled()) {
        GtkWidget *autoclick_item = gtk_menu_item_new_with_label("AutoClick all bots");
        g_signal_connect(autoclick_item, "activate",
                         G_CALLBACK(on_autoclick_activate), url_entry);
        gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), autoclick_item);

        // Optional separator (only shown when AutoClick is present)
        gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),
                              gtk_separator_menu_item_new());
    } else {
        ciao_debug("Bookmarks disabled via config.conf — skipping AutoClick and Bookmark menu");
    }

    // Bookmark Menu
    if (config_get_bookmarks_enabled()) {
        GtkWidget *bookmark_item = add_bookmark_menu_app(url_entry);
        if (bookmark_item) {
            gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), bookmark_item);
        } else {
            ciao_warn("Failed to create bookmark menu");
        }
    }

    // User Agent Menu
    GtkWidget *ua_item = add_user_agent_menu(ua_label, web_view);
    if (ua_item) {
        gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), ua_item);
    } else {
        ciao_warn("Failed to create User Agent menu");
    }

    // Exit Menu Item
    GtkWidget *exit_item = gtk_menu_item_new_with_label("Exit");
    g_signal_connect(exit_item, "activate", 
                    G_CALLBACK(on_exit_menu_item_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), exit_item);

    GtkWidget *file_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);

    // About Menu (unchanged)
    GtkWidget *about_menu = gtk_menu_new();
    GtkWidget *version_item = gtk_menu_item_new_with_label("Show Version");
    g_signal_connect(version_item, "activate", 
                    G_CALLBACK(on_show_version_activate), parent_window);
    gtk_menu_shell_append(GTK_MENU_SHELL(about_menu), version_item);

    GtkWidget *about_item = gtk_menu_item_new_with_label("About");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(about_item), about_menu);

    // Final assembly
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), about_item);

    ciao_debug("Main menu bar built successfully");
    return menu_bar;
}