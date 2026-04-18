// =========================================================================
// CIAO DEFENSIVE CODING STYLE - URL BAR CALLBACKS (CROSS-FILE COMPATIBLE)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// 
// Purpose:
//   Central declaration header for all URL bar callbacks.
//   This allows different URL bar implementations (url_bar.c, url_bar_user_agent.c,
//   or future variants) to connect signals without code duplication or modification.
//
// Flexibility Goal:
//   The same callback implementations (in url_bar_go_button.c and url_bar_page.c)
//   can be used by any project that includes this header — no changes needed.
//
// Protection Rule:
//   Future AI / developers MUST NOT add 'static' to any function declared here.
//   These callbacks require EXTERNAL LINKAGE for cross-file g_signal_connect().
//
// Last aligned with CIAO style + flexible integration: 2026-04-18
// =========================================================================

#ifndef URL_BAR_CALLBACKS_H
#define URL_BAR_CALLBACKS_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// =========================================================================
// Go Button / Enter Key Callback
// =========================================================================
void on_go_button_clicked(GtkButton *button, gpointer user_data);

// =========================================================================
// URL Bar Update Callbacks
// =========================================================================
void on_uri_changed(GObject *object, GParamSpec *pspec, gpointer user_data);
void on_load_changed(WebKitWebView *web_view, 
                     WebKitLoadEvent load_event, 
                     gpointer user_data);

#endif // URL_BAR_CALLBACKS_H