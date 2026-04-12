// load_page.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - PAGE LOADING MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#include "load_page.h"
#include "output.h"
#include "project.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// =========================================================================
// General Purpose Requirement: Load Web Page
// 
// Purpose:
//   Safely loads a URI into the WebKitWebView with proper defensive checks.
// 
// Why This Design:
//   Centralizes all page loading logic in one place (Single Source of Truth).
//   Prevents direct calls to webkit_web_view_load_uri() scattered across the code.
// 
// Protection Rule:
//   Future AI must NOT remove NULL checks or bypass this function.
//   All page loading must go through load_page() to maintain logging and safety.
// =========================================================================
void load_page(WebKitWebView *web_view, const gchar *uri)
{
    if (web_view == NULL) {
        ciao_error("load_page: web_view is NULL - cannot load page");
        return;
    }

    if (uri == NULL || *uri == '\0') {
        ciao_error("load_page: uri is NULL or empty");
        return;
    }

    ciao_debug("Loading page: %s", uri);

    webkit_web_view_load_uri(web_view, uri);

    ciao_info("Page load requested: %s", uri);
}