// load_page.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - PAGE LOADING HEADER (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR FUNCTION COMMENT BLOCK !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#ifndef LOAD_PAGE_H
#define LOAD_PAGE_H

#include <webkit2/webkit2.h>

// =========================================================================
// Public API - Page Loading Module
// 
// Purpose:
//   Provides a single, safe entry point for loading web pages into
//   a WebKitWebView. All page loading must go through this function.
// 
// Design Rules:
//   - Centralizes URI loading logic (Single Source of Truth).
//   - Defensive checks and logging are handled in the .c implementation.
// 
// Protection Rule:
//   Future AI must NOT add extra functions or remove this signature.
//   Direct calls to webkit_web_view_load_uri() are forbidden outside this module.
// =========================================================================

// =========================================================================
// Load a URI into the WebKitWebView
// 
// Parameters:
//   web_view : The target WebKitWebView instance (must not be NULL)
//   uri      : The URI to load (must not be NULL or empty)
// 
// Behavior:
//   - Logs the action through the central ciao_output system
//   - Performs defensive validation before loading
// =========================================================================
void load_page(WebKitWebView *web_view, const gchar *uri);

#endif // LOAD_PAGE_H