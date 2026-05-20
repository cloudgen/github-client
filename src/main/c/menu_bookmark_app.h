// menu_bookmark_app.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - GM BOOKMARK MENU HEADER (GNOME + GTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR FUNCTION COMMENT BLOCK !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#ifndef MENU_BOOKMARK_GITHUB_H
#define MENU_BOOKMARK_GITHUB_H

#include <gtk/gtk.h>

// =========================================================================
// Create and return the Bookmark menu item with submenu
// 
// Parameters:
//   url_entry : The main URL bar (GtkEntry) used to pass context to menu items
// 
// Returns:
//   GtkWidget* - The top-level "Bookmark" menu item (with submenu attached)
// 
// Protection:
//   Performs NULL check in implementation and logs via ciao_output system.
// =========================================================================
GtkWidget* add_bookmark_menu_app(GtkEntry *url_entry);

void on_menu_item_clicked(GtkMenuItem *menu_item, gpointer user_data);

// Callback for the AutoClick menu item (called from main.c → File menu)
void on_autoclick_activate(GtkMenuItem *item, gpointer user_data);

#endif // MENU_BOOKMARK_GITHUB_H