// =========================================================================
// CIAO DEFENSIVE CODING STYLE - WEBKIT SETUP MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Purpose:
//   Single Point of Entry for all WebKitGTK initialization including:
//   - WebKitWebContext creation
//   - Persistent cookie storage
//   - Persistent settings (User-Agent)
//   - WebKitWebView creation with applied settings
//
// Protection Rule:
//   Future AI must NOT create WebKitWebContext or WebKitWebView anywhere
//   else in the project. All WebKit setup must go through this module.
// 
// Last aligned with SyncPrjs CIAO style + Settings + Persistence: 2026-04-18
// =========================================================================

#ifndef WEBKIT_SETUP_H
#define WEBKIT_SETUP_H

#include <webkit2/webkit2.h>

// =========================================================================
// Public API - WebKit Setup
// 
// Returns:
//   A fully initialized WebKitWebView with persistent cookies and
//   persisted (or default) User-Agent already applied.
// 
// This is the Single Source of Truth for WebKit initialization.
// =========================================================================
WebKitWebView* setup_webkit_webview(void);

#endif // WEBKIT_SETUP_H