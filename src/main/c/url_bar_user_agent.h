// =========================================================================
// CIAO DEFENSIVE CODING STYLE - URL BAR + USER AGENT MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR FUNCTION COMMENT BLOCK !!!
// 
// Note: This file replaces the original url_bar.h
// Struct renamed to UrlBarUserAgent per project requirements.
// Last aligned with SyncPrjs CIAO style: 2026-04-12
// =========================================================================

#ifndef URL_BAR_USER_AGENT_H
#define URL_BAR_USER_AGENT_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// =========================================================================
// URL Bar Structure (renamed to UrlBarUserAgent)
// =========================================================================
typedef struct {
    GtkEntry  *url_entry;
    GtkWidget *url_box;           // Container for URL entry + Go button
    GtkLabel  *ua_label;          // Non-modifiable textbox showing current User-Agent
} UrlBarUserAgent;

// =========================================================================
// Public API
// 
// Purpose:
//   Creates URL bar + non-editable User-Agent label below it.
//   Returns UrlBarUserAgent struct containing all widgets.
// 
// Protection Rule:
//   Future AI must NOT change this signature or struct name.
//   All URL bar creation must go through this function.
// =========================================================================
UrlBarUserAgent add_url_bar_user_agent(WebKitWebView *web_view);

#endif // URL_BAR_USER_AGENT_H