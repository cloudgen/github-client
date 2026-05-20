// =========================================================================
// CIAO DEFENSIVE CODING STYLE - ABOUT DIALOG MODULE (GNOME + GTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR FUNCTION COMMENT BLOCK !!!
// 
// Purpose:
//   Displays a rich "About" dialog with application details, links,
//   license, author, platform, and libc information.
//
// Last aligned with SyncPrjs CIAO style: 2026-04-18
// =========================================================================

#ifndef DIALOG_ABOUT_H
#define DIALOG_ABOUT_H

#include <gtk/gtk.h>

// =========================================================================
// Public API - About Dialog
// 
// Purpose:
//   Shows the standard GNOME About dialog with rich metadata.
// 
// Protection Rule:
//   Future AI must NOT remove platform/libc detection or any metadata.
//   All About dialog logic must go through this function.
// =========================================================================
void on_show_about_activate(GtkMenuItem *menu_item, gpointer user_data);

#endif // DIALOG_ABOUT_H