// url_bar.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - URL BAR HEADER (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR FUNCTION COMMENT BLOCK !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#ifndef URL_BAR_H
#define URL_BAR_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>     // ← Required for WebKitWebView type

// =========================================================================
// URL Bar Structure
// =========================================================================
typedef struct {
    GtkEntry  *url_entry;
    GtkWidget *url_box;
} UrlBar;

// =========================================================================
// Public API - URL Bar Module
// 
// Purpose:
//   Creates a fully functional URL bar (entry + Go button) connected
//   to a WebKitWebView.
// 
// Protection Rule:
//   All URL bar creation must go through this function.
//   Future AI must NOT change this signature.
// =========================================================================
UrlBar add_url_bar(WebKitWebView *web_view);

#endif // URL_BAR_H