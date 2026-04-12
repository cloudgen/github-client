// dialog_version.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - VERSION DIALOG HEADER (GNOME + GTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR FUNCTION COMMENT BLOCK !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#ifndef DIALOG_VERSION_H
#define DIALOG_VERSION_H

#include <gtk/gtk.h>
#include "project.h"

// =========================================================================
// Public API - Version Dialog Module
// 
// Purpose:
//   Provides a callback function to display the application version
//   information in a standard GNOME message dialog.
// 
// Design Rules:
//   - Function is designed to be connected directly to a GtkMenuItem signal.
//   - user_data must be the main application window (GtkWindow*).
// 
// Protection Rule:
//   Future AI must NOT remove the function signature or add extra public
//   functions unless they follow the same defensive standard.
// =========================================================================

// =========================================================================
// Callback: Show Version Dialog
// 
// Triggered by:
//   "Version" or "About" menu item activation
// 
// Parameters:
//   menu_item  : The menu item that triggered this callback
//   user_data  : Must be castable to GtkWindow* (main window)
// =========================================================================
void on_show_version_activate(GtkMenuItem *menu_item, gpointer user_data);

#endif // DIALOG_VERSION_H