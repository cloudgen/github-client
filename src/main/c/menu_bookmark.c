// =========================================================================
// CIAO DEFENSIVE CODING STYLE - GENERIC BOOKMARK CALLBACK IMPLEMENTATION
// =========================================================================

#include "menu_bookmark.h"
#include "output.h"
#include "load_page.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// =========================================================================
// Centralized Generic Bookmark Callback
// 
// Name: on_menu_bookmark_clicked
// Purpose: Handles activation of any bookmark menu item.
// This is the Single Source of Truth for bookmark navigation logic.
// =========================================================================
void on_menu_bookmark_clicked(GtkMenuItem *menu_item, gpointer user_data)
{
    const gchar *url = (const gchar *)user_data;

    if (url == NULL || *url == '\0') {
        ciao_error("on_menu_bookmark_clicked: received empty or NULL URL");
        return;
    }

    // Retrieve URL entry from menu item data
    GtkEntry *entry = GTK_ENTRY(g_object_get_data(G_OBJECT(menu_item), "url_entry"));
    if (entry == NULL) {
        ciao_error("on_menu_bookmark_clicked: url_entry not found in menu item data");
        return;
    }

    // Retrieve web view from entry's user data
    WebKitWebView *web_view = g_object_get_data(G_OBJECT(entry), "web_view");

    if (!WEBKIT_IS_WEB_VIEW(web_view)) {
        ciao_warn("Invalid WebKitWebView reference in bookmark menu");
        return;
    }

    ciao_debug("Bookmark activated: %s", url);

    load_page(web_view, url);

    // Update URL bar
    gtk_entry_set_text(entry, url);

    ciao_info("Navigated to bookmark: %s", url);
}