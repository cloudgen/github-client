// url_bar.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - URL BAR MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#include "url_bar.h"
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
    if (load_event != WEBKIT_LOAD_FINISHED) {
        return;
    }

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
    (void)button;  // Unused parameter

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
// Internal Callback: URL Entry Changed
// =========================================================================
static void on_url_changed(GtkEntry *entry, gpointer user_data)
{
    (void)user_data;  // Unused parameter

    if (entry == NULL) {
        return;
    }

    const gchar *url = gtk_entry_get_text(entry);
    g_object_set_data(G_OBJECT(entry), "current_url", (gpointer)url);

    ciao_debug("URL entry text changed: %s", url ? url : "(empty)");
}

// =========================================================================
// General Purpose Requirement: Create URL Bar
// 
// Purpose:
//   Creates a complete URL bar with entry field and "Go" button.
// 
// Protection Rule:
//   Future AI must NOT bypass this function. All URL bar creation
//   must go through add_url_bar() to maintain signal connections and safety.
// =========================================================================
UrlBar add_url_bar(WebKitWebView *web_view)
{
    if (web_view == NULL) {
        ciao_error("add_url_bar: web_view is NULL");
        UrlBar empty = {0};
        return empty;
    }

    UrlBar url_bar = {0};

    // Create URL entry
    url_bar.url_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_text(url_bar.url_entry, START_PAGE);
    g_object_set_data(G_OBJECT(url_bar.url_entry), "web_view", web_view);

    // Create Go button
    GtkWidget *go_button = gtk_button_new_with_label("Go");
    g_signal_connect(go_button, "clicked", 
                    G_CALLBACK(on_go_button_clicked), url_bar.url_entry);

    // Connect load-changed signal to auto-update URL bar
    g_signal_connect(web_view, "load-changed", 
                    G_CALLBACK(on_load_changed), url_bar.url_entry);

    // Create horizontal box
    url_bar.url_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(url_bar.url_box), 
                      GTK_WIDGET(url_bar.url_entry), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(url_bar.url_box), go_button, FALSE, FALSE, 0);

    ciao_debug("URL bar created successfully");

    return url_bar;
}