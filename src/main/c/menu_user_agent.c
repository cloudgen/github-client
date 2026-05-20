// =========================================================================
// CIAO DEFENSIVE CODING STYLE - USER AGENT MENU MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// !!! ALL UA STRINGS MUST COME FROM project.h (Single Source of Truth) !!!
// 
// Last aligned with SyncPrjs CIAO style + User-Agent feature: 2026-04-12
// =========================================================================

#include "menu_user_agent.h"
#include "output.h"
#include "project.h"
#include "settings.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// ──────────────────────────────────────────────
// on_user_agent_activate() - Internal Callback: User Agent Menu Item Activated
// ──────────────────────────────────────────────
//
// Purpose:
//   Changes the User-Agent on the WebKitWebView and updates the display
//   label under the address bar.
//
// Parameters:
//   Input:
//     - menu_item : GtkMenuItem* - The menu item that was activated
//     - user_data : gpointer - The User-Agent string (from project.h constants)
//   Output/Return: None (void callback)
//
// Dependencies:
//   - Functions called: ciao_error(), ciao_warn(), ciao_info(), ciao_debug(),
//     webkit_web_view_get_settings(), webkit_settings_set_user_agent(),
//     gtk_label_set_text(), save_user_agent_to_settings(), g_object_get_data()
//   - Headers required: gtk/gtk.h, webkit2/webkit2.h, menu_user_agent.h, output.h, settings.h, project.h
//   - External: Connected from User Agent menu items
//
// Protection Rule:
//   Future AI must NOT remove NULL checks or bypass ciao_* logging.
//
// CIAO Principles Applied:
//   - Caution: Duplicate NULL check + full validation of UA string, label and web_view
//   - Intentional: Updates both WebKit settings and UI label + persists to settings.sqlite
//   - Anti-fragile: Graceful degradation on missing settings or invalid references
//   - Over-protect: All User-Agent changes go through this centralized handler
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
static void on_user_agent_activate(GtkMenuItem *menu_item, gpointer user_data)
{
    const gchar *ua_string = (const gchar *)user_data;

    if (ua_string == NULL || *ua_string == '\0') {
        ciao_error("on_user_agent_activate: received empty or NULL User-Agent");
        return;
    }

    if (ua_string == NULL || *ua_string == '\0') {
        ciao_error("on_user_agent_activate: received empty or NULL User-Agent");
        return;
    }

    // Retrieve ua_label from menu item data
    GtkLabel *ua_label = GTK_LABEL(g_object_get_data(G_OBJECT(menu_item), "ua_label"));
    if (ua_label == NULL) {
        ciao_error("on_user_agent_activate: ua_label not found in menu item data");
        return;
    }

    // Retrieve web_view from menu item data
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(menu_item), "web_view"));
    if (!WEBKIT_IS_WEB_VIEW(web_view)) {
        ciao_warn("Invalid WebKitWebView reference in User Agent menu");
        return;
    }

    // Apply new User-Agent to WebKit
    WebKitSettings *settings = webkit_web_view_get_settings(web_view);
    if (settings != NULL) {
        webkit_settings_set_user_agent(settings, ua_string);
        ciao_info("User-Agent changed to: %s", ua_string);
    } else {
        ciao_warn("Failed to get WebKitSettings");
    }

    // Update the non-modifiable label under the address bar
    gtk_label_set_text(ua_label, ua_string);

    // === NEW: Persist the change to settings.sqlite ===
    save_user_agent_to_settings(ua_string);

    ciao_debug("User-Agent menu activated: %s", ua_string);
}

// ──────────────────────────────────────────────
// add_user_agent_menu() - General Purpose Requirement: Create User Agent Menu
// ──────────────────────────────────────────────
//
// Purpose:
//   Creates a "User Agent" submenu with predefined browser identities.
//   Google Chrome is the default (as requested).
//
// Parameters:
//   Input:
//     - ua_label : GtkLabel* - The non-editable label under the address bar
//     - web_view : WebKitWebView* - The target web view (must be valid)
//   Output/Return:
//     - GtkWidget* - The top-level "User Agent" menu item with submenu attached
//
// Dependencies:
//   - Functions called: gtk_menu_new(), gtk_menu_item_new_with_label(),
//     g_object_set_data(), g_signal_connect_data(), on_user_agent_activate(),
//     ciao_error(), ciao_debug()
//   - Headers required: gtk/gtk.h, webkit2/webkit2.h, menu_user_agent.h, project.h, output.h
//   - Constants: All UA_xxx strings from project.h (Single Source of Truth)
//
// Why This Design:
//   Uses GObject data attachment pattern for context passing and connects
//   all menu items to the centralized handler.
//
// Protection Rule:
//   All UA strings MUST come from project.h.
//   Never hardcode UA strings here.
//
// CIAO Principles Applied:
//   - Caution: Full NULL validation on inputs
//   - Intentional: Single Point of Truth for User Agent menu creation
//   - Anti-fragile: Safe early returns on invalid parameters
//   - Over-protect: Enforces use of project.h constants
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
GtkWidget* add_user_agent_menu(GtkLabel *ua_label, WebKitWebView *web_view)
{
    if (ua_label == NULL) {
        ciao_error("add_user_agent_menu: ua_label is NULL");
        return NULL;
    }

    if (web_view == NULL || !WEBKIT_IS_WEB_VIEW(web_view)) {
        ciao_error("add_user_agent_menu: web_view is NULL or invalid");
        return NULL;
    }

    GtkWidget *ua_menu = gtk_menu_new();

    // Create menu items using constants from project.h
    GtkWidget *webview_item  = gtk_menu_item_new_with_label("Webview (Default)");
    GtkWidget *chrome_item  = gtk_menu_item_new_with_label("Google Chrome");
    GtkWidget *firefox_item = gtk_menu_item_new_with_label("Mozilla Firefox");
    GtkWidget *safari_item  = gtk_menu_item_new_with_label("Apple Safari");
    GtkWidget *edge_item    = gtk_menu_item_new_with_label("Microsoft Edge");

    // Attach context data to each menu item (CIAO pattern used in bookmark menu)
    g_object_set_data(G_OBJECT(webview_item),  "ua_label", ua_label);
    g_object_set_data(G_OBJECT(webview_item),  "web_view", web_view);

    g_object_set_data(G_OBJECT(chrome_item),  "ua_label", ua_label);
    g_object_set_data(G_OBJECT(chrome_item),  "web_view", web_view);

    g_object_set_data(G_OBJECT(firefox_item), "ua_label", ua_label);
    g_object_set_data(G_OBJECT(firefox_item), "web_view", web_view);

    g_object_set_data(G_OBJECT(safari_item),  "ua_label", ua_label);
    g_object_set_data(G_OBJECT(safari_item),  "web_view", web_view);

    g_object_set_data(G_OBJECT(edge_item),    "ua_label", ua_label);
    g_object_set_data(G_OBJECT(edge_item),    "web_view", web_view);

    // Connect signals with UA strings from project.h
    g_signal_connect_data(webview_item, "activate",
                         G_CALLBACK(on_user_agent_activate),
                         (gpointer)UA_WEBVIEW, NULL, 0);

    g_signal_connect_data(chrome_item, "activate",
                         G_CALLBACK(on_user_agent_activate),
                         (gpointer)UA_CHROME, NULL, 0);

    g_signal_connect_data(firefox_item, "activate",
                         G_CALLBACK(on_user_agent_activate),
                         (gpointer)UA_FIREFOX, NULL, 0);

    g_signal_connect_data(safari_item, "activate",
                         G_CALLBACK(on_user_agent_activate),
                         (gpointer)UA_SAFARI, NULL, 0);

    g_signal_connect_data(edge_item, "activate",
                         G_CALLBACK(on_user_agent_activate),
                         (gpointer)UA_EDGE, NULL, 0);

    // Build menu structure
    gtk_menu_shell_append(GTK_MENU_SHELL(ua_menu), webview_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(ua_menu), chrome_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(ua_menu), firefox_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(ua_menu), safari_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(ua_menu), edge_item);

    // Top-level menu item
    GtkWidget *ua_menu_item = gtk_menu_item_new_with_label("User Agent");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(ua_menu_item), ua_menu);

    ciao_debug("User Agent menu created successfully (default: Google Chrome)");

    return ua_menu_item;
}

