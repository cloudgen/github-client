// =========================================================================
// CIAO DEFENSIVE CODING STYLE - USER AGENT MENU MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR FUNCTION COMMENT BLOCK !!!
// 
// Purpose:
//   Provides a submenu under File → User Agent to spoof common browser
//   User-Agents (Google Chrome as default, Firefox, Safari, Microsoft Edge).
//
// Last aligned with SyncPrjs CIAO style + User-Agent feature: 2026-04-12
// =========================================================================

#ifndef MENU_USER_AGENT_H
#define MENU_USER_AGENT_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// =========================================================================
// Public API - User Agent Menu Module
// 
// Purpose:
//   Creates a "User Agent" submenu that allows switching between common
//   browser User-Agents. Updates both the WebKitWebView and the
//   non-modifiable ua_label under the address bar.
//
// Protection Rule:
//   Future AI must NOT change this function signature or expose internal
//   callbacks. All User Agent menu creation must go through this function.
// =========================================================================
GtkWidget* add_user_agent_menu(GtkLabel *ua_label, WebKitWebView *web_view);

#endif // MENU_USER_AGENT_H