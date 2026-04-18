// =========================================================================
// CIAO DEFENSIVE CODING STYLE - GENERIC BOOKMARK CALLBACK MODULE
// =========================================================================
//
// Purpose:
//   Provides a reusable bookmark callback that can be used across
//   different projects (YouTube, GitHub, etc.).
//
// Protection Rule:
//   This file contains only generic logic. No project-specific code.
// =========================================================================

#ifndef MENU_BOOKMARK_H
#define MENU_BOOKMARK_H

#include <gtk/gtk.h>

// =========================================================================
// Generic Bookmark Callback
//
// Renamed to on_menu_bookmark_clicked for better clarity and intent.
// This callback is designed to be connected from any project-specific
// bookmark menu file.
// =========================================================================
void on_menu_bookmark_clicked(GtkMenuItem *menu_item, gpointer user_data);

#endif // MENU_BOOKMARK_H