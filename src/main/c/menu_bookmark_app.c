// =========================================================================
// CIAO DEFENSIVE CODING STYLE - GM BOOKMARK MENU MODULE (GNOME + GTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#include "menu_bookmark_app.h"
#include "menu_bookmark.h"
#include "auto_follow.h"          // Fixed: missing include for inject_auto_follow_script
#include "output.h"
#include "project.h"
#include "load_page.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// ──────────────────────────────────────────────
// autoclick_bot_urls - AutoClick Bot URLs Array
// ──────────────────────────────────────────────
//
// Purpose:
//   Static array containing all Github bot URLs that should be auto-clicked
//   during the AutoClick sequence (excludes START_PAGE and CONTACTS_PAGE).
//
// Parameters:
//   Input: None (static constant array)
//   Output/Return: None (used internally by autoclick_next_page())
//
// Dependencies:
//   - Constants from: project.h (CIRCUIT_ANALYSIS, ESP32_MASTER, etc.)
//   - Used by: autoclick_next_page(), on_autoclick_activate()
//
// Why This Design:
//   Centralized list for easy maintenance of which bots participate in
//   the auto-click sequence. Sentinel NULL value marks the end.
//
// CIAO Principles Applied:
//   - Intentional: Single source of bot list for AutoClick feature
//   - Anti-fragile: Clear exclusion of Start/API pages
//   - Over-protect: Static definition prevents accidental modification
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Remove any bot from this list without explicit instruction
//     - Add hardcoded URLs instead of project.h constants
//   This array defines the official AutoClick scope.
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
static const char *autoclick_bot_urls[] = {
    NULL   // sentinel value
};

// ──────────────────────────────────────────────
// AutoClick State Variables (File Scope)
// ──────────────────────────────────────────────
//
// Purpose:
//   Maintain global state for the AutoClick sequence across timer callbacks.
//
// Parameters:
//   Input/Output: Static file-scope variables (autoclick_timeout_id, etc.)
//
// Dependencies:
//   - Used by: on_autoclick_activate(), autoclick_next_page(), delayed_inject()
//
// Why This Design:
//   File-scope statics keep AutoClick state encapsulated within this module.
//
// Protection Rule (Sacred):
//   Do NOT move these variables outside this file or convert to global.
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
static guint autoclick_timeout_id = 0;
static int   autoclick_current_index = -1;
static WebKitWebView *autoclick_web_view = NULL;
static GtkEntry      *autoclick_url_entry = NULL;

// ──────────────────────────────────────────────
// delayed_inject() - Auto-Follow Injection (Delayed)
// ──────────────────────────────────────────────
//
// Purpose:
//   Delayed callback to inject the auto-follow script after page load.
//
// Parameters:
//   Input:
//     - user_data : gpointer - Expected to be WebKitWebView*
//   Output/Return:
//     - gboolean - Always G_SOURCE_REMOVE (one-shot)
//
// Dependencies:
//   - Functions called: inject_auto_follow_script(), WEBKIT_IS_WEB_VIEW()
//   - Headers required: webkit2/webkit2.h, auto_follow.h
//
// Why This Design:
//   Small delay ensures the page has rendered before attempting to click Follow.
//
// Protection Rule (Sacred):
//   Do NOT inline or remove this delayed wrapper.
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
static gboolean delayed_inject(gpointer user_data)
{
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(user_data);

    if (WEBKIT_IS_WEB_VIEW(web_view)) {
        inject_auto_follow_script(web_view);
    }

    return G_SOURCE_REMOVE;
}

// ──────────────────────────────────────────────
// on_load_finished() - Page Load Finished Handler for Auto-Follow
// ──────────────────────────────────────────────
//
// Purpose:
//   Called when a page finishes loading to schedule auto-follow injection.
//
// Parameters:
//   Input:
//     - web_view    : WebKitWebView*
//     - load_event  : WebKitLoadEvent
//     - user_data   : gpointer (unused)
//   Output/Return: None
//
// Dependencies:
//   - Functions called: delayed_inject(), g_timeout_add(), g_signal_handlers_disconnect_by_func()
//   - Headers required: webkit2/webkit2.h
//
// Why This Design:
//   One-time connection per page to trigger auto-follow after full load.
//
// Protection Rule (Sacred):
//   Do NOT remove the disconnect logic to prevent multiple handlers.
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
static void on_load_finished(WebKitWebView *web_view,
                             WebKitLoadEvent load_event,
                             gpointer user_data)
{
    if (load_event == WEBKIT_LOAD_FINISHED) {
        g_timeout_add(100, delayed_inject, web_view);
        g_signal_handlers_disconnect_by_func(web_view, on_load_finished, NULL);
    }
}

// ──────────────────────────────────────────────
// autoclick_next_page() - AutoClick Sequence Step
// ──────────────────────────────────────────────
//
// Purpose:
//   Loads the next bot page in the AutoClick sequence and advances the index.
//
// Parameters:
//   Input:
//     - user_data : gpointer - Expected to be GtkEntry*
//   Output/Return:
//     - gboolean - G_SOURCE_CONTINUE (until sequence ends) or G_SOURCE_REMOVE
//
// Dependencies:
//   - Functions called: load_page(), gtk_entry_set_text(), g_print()
//   - Headers required: gtk/gtk.h, load_page.h
//
// Why This Design:
//   Timer-driven sequential navigation through all bots with 4-second intervals.
//
// Protection Rule (Sacred):
//   Do NOT modify the termination logic or sentinel check.
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
static gboolean autoclick_next_page(gpointer user_data)
{
    GtkEntry *entry = GTK_ENTRY(user_data);

    if (autoclick_current_index < 0 ||
        autoclick_bot_urls[autoclick_current_index] == NULL ||
        !WEBKIT_IS_WEB_VIEW(autoclick_web_view)) {
        autoclick_timeout_id = 0;
        autoclick_current_index = -1;
        autoclick_web_view = NULL;
        autoclick_url_entry = NULL;
        g_print("[AUTOCLICK] Sequence finished or cancelled\n");
        return G_SOURCE_REMOVE;
    }

    const char *url = autoclick_bot_urls[autoclick_current_index];
    g_print("[AUTOCLICK] %2d → %s\n", autoclick_current_index, url);

    load_page(autoclick_web_view, url);
    if (entry) {
        gtk_entry_set_text(entry, url);
    }

    autoclick_current_index++;
    return G_SOURCE_CONTINUE;
}

// ──────────────────────────────────────────────
// on_autoclick_activate() - Start AutoClick Sequence
// ──────────────────────────────────────────────
//
// Purpose:
//   Menu callback that starts the full AutoClick sequence through all bots.
//
// Parameters:
//   Input:
//     - item      : GtkMenuItem*
//     - user_data : gpointer - Expected to be GtkEntry*
//   Output/Return: None
//
// Dependencies:
//   - Functions called: autoclick_next_page(), g_timeout_add_seconds(), g_source_remove()
//   - Headers required: gtk/gtk.h, menu_bookmark_app.h
//
// Why This Design:
//   Provides one-click automation for testing / demoing multiple bots.
//
// Protection Rule (Sacred):
//   Do NOT remove existing sequence cancellation logic.
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
void on_autoclick_activate(GtkMenuItem *item, gpointer user_data)
{
    GtkEntry *url_entry = GTK_ENTRY(user_data);

    WebKitWebView *web_view = g_object_get_data(G_OBJECT(url_entry), "web_view");
    if (!WEBKIT_IS_WEB_VIEW(web_view)) {
        g_warning("[AUTOCLICK] Cannot find valid web_view");
        return;
    }

    // Stop any existing sequence
    if (autoclick_timeout_id != 0) {
        g_source_remove(autoclick_timeout_id);
        autoclick_timeout_id = 0;
    }

    autoclick_web_view   = web_view;
    autoclick_url_entry  = url_entry;
    autoclick_current_index = 0;

    // Load first page immediately
    autoclick_next_page(url_entry);

    // Schedule the rest
    autoclick_timeout_id = g_timeout_add_seconds(4, autoclick_next_page, url_entry);

    g_print("[AUTOCLICK] Sequence started\n");
}

// ──────────────────────────────────────────────
// on_menu_item_clicked() - Single Bookmark Click Handler
// ──────────────────────────────────────────────
//
// Purpose:
//   Generic callback for individual bookmark menu items.
//
// Parameters:
//   Input:
//     - menu_item : GtkMenuItem*
//     - user_data : gpointer - The target URL string
//   Output/Return: None
//
// Dependencies:
//   - Functions called: load_page(), gtk_entry_set_text()
//   - Headers required: gtk/gtk.h, load_page.h, menu_bookmark.h
//
// Protection Rule (Sacred):
//   Do NOT bypass this handler for bookmark navigation.
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
void on_menu_item_clicked(GtkMenuItem *menu_item, gpointer user_data)
{
    const gchar *url = (const gchar *)user_data;
    GtkEntry *entry = g_object_get_data(G_OBJECT(menu_item), "url_entry");

    WebKitWebView *web_view = g_object_get_data(G_OBJECT(entry), "web_view");
    if (WEBKIT_IS_WEB_VIEW(web_view)) {
        load_page(web_view, url);
        gtk_entry_set_text(entry, url);
    }
}

// ──────────────────────────────────────────────
// add_bookmark_menu_app() - Create Github Bookmark Menu
// ──────────────────────────────────────────────
//
// Purpose:
//   Creates and returns the complete "Bookmark" menu with all Github-specific entries.
//   This is the Single Point of Truth for the Github bookmark menu structure.
//
// Parameters:
//   Input:
//     - url_entry : GtkEntry* - The main URL bar (must not be NULL)
//   Output/Return:
//     - GtkWidget* - The top-level "Bookmark" menu item with submenu
//
// Dependencies:
//   - Functions called: gtk_menu_new(), gtk_menu_item_new_with_label(),
//     g_signal_connect_data(), on_menu_item_clicked(), ciao_debug()
//   - Headers required: gtk/gtk.h, menu_bookmark_app.h, project.h, output.h
//   - Macros: ADD_BOOKMARK (local)
//
// Why This Design:
//   Centralized menu construction using project.h constants (Single Source of Truth).
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Remove or rename this function
//     - Hardcode URLs instead of project.h constants
//     - Bypass this function for creating the bookmark menu
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
GtkWidget* add_bookmark_menu_app(GtkEntry *url_entry)
{
    if (url_entry == NULL) {
        ciao_error("add_bookmark_menu_app: url_entry is NULL");
        return NULL;
    }

    GtkWidget *bookmark_menu = gtk_menu_new();

    #define ADD_BOOKMARK(label, url) do { \
        GtkWidget *item = gtk_menu_item_new_with_label(label); \
        g_signal_connect_data(item, "activate", G_CALLBACK(on_menu_item_clicked), \
                              (gpointer)(url), NULL, 0); \
        g_object_set_data(G_OBJECT(item), "url_entry", url_entry); \
        gtk_menu_shell_append(GTK_MENU_SHELL(bookmark_menu), item); \
    } while (0)

    ADD_BOOKMARK("Start Page",      START_PAGE);

    #undef ADD_BOOKMARK

    GtkWidget *bookmark_menu_item = gtk_menu_item_new_with_label("Bookmark");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(bookmark_menu_item), bookmark_menu);

    ciao_debug("Github bookmark menu created successfully");
    return bookmark_menu_item;
}

