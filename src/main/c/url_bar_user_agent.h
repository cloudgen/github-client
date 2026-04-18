// url_bar_user_agent.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - URL BAR + USER AGENT MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last reviewed & aligned with CIAO 18 Principles: April 2026
// =========================================================================

#ifndef URL_BAR_USER_AGENT_H
#define URL_BAR_USER_AGENT_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// =========================================================================
// UrlBarUserAgent Structure
// 
// Purpose:
//   Holds all widgets related to the URL bar and User-Agent display.
//   This is the Single Source of Truth for the combined URL bar component.
//
// Protection Rule:
//   Future AI must NOT rename this struct or change its fields without
//   updating all call sites.
// =========================================================================
typedef struct {
    GtkEntry  *url_entry;
    GtkWidget *url_box;           // Container for URL entry + Go button
    GtkLabel  *ua_label;          // Non-modifiable label showing current User-Agent
} UrlBarUserAgent;

// =========================================================================
// Public API
// 
// Purpose:
//   Creates the complete URL bar with Go button and User-Agent label.
//   This is the Single Point of Entry for URL bar creation.
// 
// Protection Rule:
//   All signal connections must be established here. Do not move them.
// =========================================================================
UrlBarUserAgent add_url_bar_user_agent(WebKitWebView *web_view, 
                                       const char *initial_ua);

#endif // URL_BAR_USER_AGENT_H