// ui_menu_github.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - UI MENU IMPLEMENTATION (GITHUB SPECIFIC)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + Strong Protection: 2026-04-18
// =========================================================================

#include "ui_menu_github.h"
#include "menu_bookmark_github.h"
#include "menu_user_agent.h"
#include "menu_exit.h"
#include "dialog_version.h"
#include "output.h"

// =========================================================================
// General Purpose Requirement: Build Main Menu Bar
// 
// Purpose:
//   Creates and assembles the complete menu bar (File → Bookmark, User Agent,
//   Exit | About → Show Version).
//
// CIAO Protection Rule (Sacred - Do Not Violate):
//   This is the **Single Point of Truth** for menu construction in the GitHub client.
// 
//   Future AI assistants or developers **MUST NOT**:
//     - Remove or weaken any NULL checks
//     - Inline this function into ui_window_github.c or main.c
//     - Rename this function
//     - Simplify the menu structure without creating a new project-specific module
//     - Move any menu item creation logic outside this file
// 
//   Reason for Protection:
//     Menu logic tends to grow and become project-specific. Keeping it isolated
//     makes forking to YouTube/GitLab/etc. much safer and cleaner.
//
//   This follows CIAO Principle 5 (Single Point of Entry) + Principle 18
//   (Protect Against AI & Human Modification).
// =========================================================================
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

    // Bookmark Menu
    GtkWidget *bookmark_item = add_bookmark_menu_github(url_entry);
    if (bookmark_item) {
        gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), bookmark_item);
    } else {
        ciao_warn("Failed to create bookmark menu");
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

    // About Menu
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

    ciao_debug("Main menu bar built successfully (GitHub specific)");
    return menu_bar;
}