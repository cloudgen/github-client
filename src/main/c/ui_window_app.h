// ui_window_app.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - UI WINDOW MODULE (GMAIL SPECIFIC)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR FUNCTION COMMENT BLOCK !!!
// 
// Protection Rule (Sacred):
//   Keep "_app" suffix. Do not rename to generic ui_window.*
// 
// Last aligned with SyncPrjs CIAO style: 2026-04-18
// =========================================================================

#ifndef UI_WINDOW_GITHUB_H
#define UI_WINDOW_GITHUB_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include "url_bar_user_agent.h"
#include "settings.h"

// =========================================================================
// Public API - Create Main Window
// 
// Protection Rule (Critical - Sacred):
//   Future AI must NOT rename, inline, or simplify create_main_window().
//   This is the Single Point of Truth for all UI layout and widget packing
//   in the Github client.
// =========================================================================
GtkWidget* create_main_window(WebKitWebView *web_view, UrlBarUserAgent *out_url_bar);

#endif // UI_WINDOW_GITHUB_H