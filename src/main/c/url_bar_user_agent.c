// =========================================================================
// CIAO DEFENSIVE CODING STYLE - URL BAR + USER AGENT MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Note: This file replaces the original url_bar.c
// Struct renamed to UrlBarUserAgent as requested.
// Last aligned with SyncPrjs CIAO style + User-Agent feature: 2026-04-12
// =========================================================================

#include "url_bar_user_agent.h"
#include "output.h"
#include "project.h"
#include "load_page.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// =========================================================================
// Internal Callback: Web Page Load Changed
// =========================================================================
static void on_load_changed(WebKitWebView *web_view, 
                           WebKitLoadEvent load_event, 
                           gpointer user_data)
{
    if (load_event != WEBKIT_LOAD_FINISHED) return;

    const gchar *url = webkit_web_view_get_uri(web_view);
    if (url == NULL) {
        ciao_warn("on_load_changed: webkit_web_view_get_uri returned NULL");
        return;
    }

    GtkEntry *entry = GTK_ENTRY(user_data);
    if (entry == NULL) {
        ciao_error("on_load_changed: url_entry is NULL");
        return;
    }

    gtk_entry_set_text(entry, url);
    ciao_debug("URL bar updated after page load: %s", url);
}

// =========================================================================
// Internal Callback: Go Button Clicked
// =========================================================================
static void on_go_button_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;

    GtkEntry *entry = GTK_ENTRY(user_data);
    if (entry == NULL) {
        ciao_error("on_go_button_clicked: url_entry is NULL");
        return;
    }

    const gchar *url = gtk_entry_get_text(entry);
    if (url == NULL || *url == '\0') {
        ciao_warn("Go button clicked with empty URL");
        return;
    }

    WebKitWebView *web_view = g_object_get_data(G_OBJECT(entry), "web_view");
    if (!WEBKIT_IS_WEB_VIEW(web_view)) {
        ciao_warn("Invalid WebKitWebView reference in URL bar");
        return;
    }

    ciao_info("Navigating to: %s", url);
    load_page(web_view, url);
}

// =========================================================================
// General Purpose Requirement: Create URL Bar with User-Agent Display
// 
// Purpose:
//   Creates URL entry + Go button, plus a non-editable GtkLabel under it
//   showing the current User-Agent string.
// 
// Protection Rule:
//   Future AI must NOT bypass this function or rename UrlBarUserAgent struct.
//   UA label must remain non-editable and is updated by the User Agent menu.
// =========================================================================
UrlBarUserAgent add_url_bar_user_agent(WebKitWebView *web_view)
{
    if (web_view == NULL) {
        ciao_error("add_url_bar_user_agent: web_view is NULL");
        UrlBarUserAgent empty = {0};
        return empty;
    }

    UrlBarUserAgent url_bar = {0};

    // Create URL entry + Go button
    url_bar.url_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_text(url_bar.url_entry, START_PAGE);
    g_object_set_data(G_OBJECT(url_bar.url_entry), "web_view", web_view);

    GtkWidget *go_button = gtk_button_new_with_label("Go");
    g_signal_connect(go_button, "clicked", 
                    G_CALLBACK(on_go_button_clicked), url_bar.url_entry);

    // URL box (horizontal)
    url_bar.url_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(url_bar.url_box), 
                      GTK_WIDGET(url_bar.url_entry), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(url_bar.url_box), go_button, FALSE, FALSE, 0);

    // === Non-modifiable User-Agent label under the address bar ===
    url_bar.ua_label = GTK_LABEL(gtk_label_new(DEFAULT_USER_AGENT));
    gtk_label_set_selectable(url_bar.ua_label, TRUE);           // Allow user to copy
    gtk_label_set_ellipsize(url_bar.ua_label, PANGO_ELLIPSIZE_END);
    gtk_widget_set_tooltip_text(GTK_WIDGET(url_bar.ua_label), 
                               "Current User-Agent (read-only)");

    ciao_debug("URL bar with User-Agent display created successfully (UrlBarUserAgent)");

    return url_bar;
}