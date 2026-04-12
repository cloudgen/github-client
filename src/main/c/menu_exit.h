// menu_exit.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - APPLICATION EXIT MENU HEADER (GNOME + GTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR FUNCTION COMMENT BLOCK !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#ifndef MENU_EXIT_H
#define MENU_EXIT_H

#include <gtk/gtk.h>

// =========================================================================
// Public API - Exit Menu Module
// 
// Purpose:
//   Provides callback for the "Exit" menu item to cleanly terminate
//   the application.
// 
// Protection Rule:
//   Future AI must NOT change function signature. All exit logic
//   must be centralized in this module.
// =========================================================================
void on_exit_menu_item_activate(GtkMenuItem *menu_item, gpointer user_data);

#endif // MENU_EXIT_H