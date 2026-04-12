// menu_bookmark_github.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - GITHUB BOOKMARK MENU MODULE (GNOME + GTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// !!! ALL WARNINGS AND LOGS MUST USE ciao_* SYSTEM !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#include "menu_bookmark_github.h"
#include "output.h"
#include "project.h"
#include "load_page.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// =========================================================================
// Internal Callback: Bookmark Menu Item Activated
// 
// Purpose:
//   Loads the selected bookmark URL and updates the URL bar.
// 
// Protection Rule:
//   Future AI must NOT remove NULL checks or bypass load_page().
// =========================================================================
static void on_menu_item_clicked(GtkMenuItem *menu_item, gpointer user_data)
{
    const gchar *url = (const gchar *)user_data;

    if (url == NULL || *url == '\0') {
        ciao_error("on_menu_item_clicked: received empty or NULL URL");
        return;
    }

    // Retrieve URL entry from menu item data
    GtkEntry *entry = GTK_ENTRY(g_object_get_data(G_OBJECT(menu_item), "url_entry"));
    if (entry == NULL) {
        ciao_error("on_menu_item_clicked: url_entry not found in menu item data");
        return;
    }

    // Retrieve web view from entry's user data
    WebKitWebView *web_view = g_object_get_data(G_OBJECT(entry), "web_view");

    if (!WEBKIT_IS_WEB_VIEW(web_view)) {
        ciao_warn("Invalid WebKitWebView reference in bookmark menu");
        return;
    }

    ciao_debug("Bookmark activated: %s", url);

    load_page(web_view, url);

    // Update URL bar
    gtk_entry_set_text(entry, url);

    ciao_info("Navigated to bookmark: %s", url);
}

// =========================================================================
// General Purpose Requirement: Create GitHub Bookmark Menu
// 
// Purpose:
//   Creates a "Bookmark" submenu with predefined GitHub-related links.
// 
// Returns:
//   GtkWidget* pointing to the top-level "Bookmark" menu item.
// =========================================================================
GtkWidget* add_bookmark_menu_yt(GtkEntry *url_entry)
{
    if (url_entry == NULL) {
        ciao_error("add_bookmark_menu_yt: url_entry is NULL");
        return NULL;
    }

    GtkWidget *bookmark_menu = gtk_menu_new();
    GtkWidget *start_menu_item   = gtk_menu_item_new_with_label("Start Page");
    GtkWidget *profile_menu_item = gtk_menu_item_new_with_label("Profile Page");

    // Attach URL entry reference to menu items
    g_object_set_data(G_OBJECT(start_menu_item),   "url_entry", url_entry);
    g_object_set_data(G_OBJECT(profile_menu_item), "url_entry", url_entry);

    // Connect signals with bookmark URLs from project.h
    g_signal_connect_data(start_menu_item,   "activate",
                         G_CALLBACK(on_menu_item_clicked),
                         (gpointer)START_PAGE, NULL, 0);

    g_signal_connect_data(profile_menu_item, "activate",
                         G_CALLBACK(on_menu_item_clicked),
                         (gpointer)PROFILE_PAGE, NULL, 0);

    // Build menu structure
    gtk_menu_shell_append(GTK_MENU_SHELL(bookmark_menu), start_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(bookmark_menu), profile_menu_item);

    GtkWidget *bookmark_menu_item = gtk_menu_item_new_with_label("Bookmark");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(bookmark_menu_item), bookmark_menu);

    ciao_debug("GitHub bookmark menu created successfully");

    return bookmark_menu_item;
}