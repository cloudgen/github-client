// menu_bookmark_github.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - GITHUB BOOKMARK MENU HEADER (GNOME + GTK)
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
// Public API - GitHub Bookmark Menu Module
// 
// Purpose:
//   Provides a bookmark submenu with quick links to important GitHub pages
//   (Start Page, Profile Page, etc.).
// 
// Design Rules:
//   - Only public function is exposed.
//   - Internal callback (on_menu_item_clicked) is hidden in .c file.
//   - Uses data passing via g_object_set_data for clean GTK integration.
// 
// Protection Rule:
//   Future AI must NOT expose internal functions or change the public
//   function signature. All menu creation must go through this module.
// =========================================================================

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
GtkWidget* add_bookmark_menu_yt(GtkEntry *url_entry);

#endif // MENU_BOOKMARK_GITHUB_H