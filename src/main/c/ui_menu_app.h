// ui_menu_app.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - UI MENU MODULE (GMAIL SPECIFIC)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR FUNCTION COMMENT BLOCK !!!
// 
// Purpose:
//   Builds the complete menu bar for the Github client (File + About menus).
//
// Protection Rule (Sacred - Do Not Violate):
//   This module is Github-specific. Future AI or developers MUST NOT rename
//   this file to a generic "ui_menu.*" name. Other projects (YouTube, GitLab, etc.)
//   should have their own ui_menu_* files to avoid mixing project-specific logic.
//
// Last aligned with SyncPrjs CIAO style: 2026-04-18
// =========================================================================

#ifndef UI_MENU_GITHUB_H
#define UI_MENU_GITHUB_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// =========================================================================
// Public API - Build Main Menu Bar
// 
// Protection Rule (Critical - Sacred):
//   Future AI / developers MUST NOT:
//     - Rename this function
//     - Change its signature
//     - Inline or simplify its logic
//     - Move menu building code back into main.c or ui_window_app.c
// 
//   This function is the Single Point of Truth for all menu construction
//   in this Github client. It must remain self-contained and defensive.
// 
//   Reason: Different projects will have very different menu structures.
// =========================================================================
GtkWidget* build_main_menu_bar(GtkEntry *url_entry, 
                               GtkLabel *ua_label, 
                               WebKitWebView *web_view,
                               GtkWindow *parent_window);

#endif // UI_MENU_GITHUB_H