// load_page_login_check.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - PAGE LOADING MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-05-02
// =========================================================================

#include "load_page_login_check.h"
#include "login_detector_app.h"
#include "output.h"
#include "project.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// ──────────────────────────────────────────────
// load_page() - Load Web Page (Core Safe Loader)
// ──────────────────────────────────────────────
//
// Purpose:
//   Safely loads a URI into the WebKitWebView with proper defensive checks.
//   This is the Single Point of Truth for all page loading in the application.
//
// Parameters:
//   Input:
//     - web_view : WebKitWebView* - Target web view (must not be NULL)
//     - uri      : const gchar* - The URI to load (must not be NULL or empty)
//   Output/Return: None (void function)
//
// Dependencies:
//   - Functions called: ciao_debug(), ciao_error(), ciao_info(), webkit_web_view_load_uri()
//   - Headers required: webkit2/webkit2.h, load_page.h, output.h, project.h
//   - External: Central entry point used by all navigation (bookmarks, URL bar, etc.)
//
// Why This Design:
//   Centralizes all page loading logic in one place to prevent scattered
//   direct calls to webkit_web_view_load_uri() and ensure consistent logging.
//
// CIAO Principles Applied:
//   - Caution: Full NULL and empty-string validation
//   - Intentional: Single Point of Truth for page loading
//   - Anti-fragile: Graceful error reporting without crashing
//   - Over-protect: Enforces logging and safety for all navigation
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Remove NULL checks or defensive validation
//     - Bypass this function for any page loading
//     - Remove ciao_* logging calls
//   All page loading must go through load_page() to maintain logging and safety.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
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


// ──────────────────────────────────────────────
// load_page_with_login_check() - Smart Load with Auto-Login Support
// ──────────────────────────────────────────────
//
// Purpose:
//   Extended version of load_page() that automatically enables login detection
//   for the START_PAGE. This is the Single Point of Truth for all Github start page loads.
//
// Parameters:
//   Input:
//     - web_view : WebKitWebView* - Target web view (must not be NULL)
//     - uri      : const gchar* - The URI to load (must not be NULL or empty)
//   Output/Return: None (void function)
//
// Dependencies:
//   - Functions called: load_page(), on_login_page_check(), g_signal_connect(),
//     g_object_set_data(), g_str_has_prefix(), ciao_info(), ciao_error()
//   - Headers required: load_page_login_check.h, login_detector_app.h, project.h
//   - External signals: Connects "load-changed" handler for auto-login
//
// Why This Design:
//   Centralizes the decision to enable login handler for Github domain.
//   Uses robust g_str_has_prefix() instead of exact matching to handle URI variations.
//
// Lessons Learned:
//   Exact string matching to START_PAGE was fragile due to WebKit URI normalization.
//   Connecting signal before load ensures handler is ready for SPA content.
//
// CIAO Principles Applied:
//   - Caution: Full parameter validation
//   - Intentional: Clear purpose and connection logic
//   - Anti-fragile: Handler stays connected for safety
//   - Single Point of Truth: All Github auto-login starts here
//
//   === ANTI-HARDCODING RULE (CRITICAL) ===
//   **NEVER HARD CODE ANY URL** inside this function (or any related login/start-page logic).
//   Always use the `START_PAGE` constant from project.h (Single Source of Truth).
//
//   Previous Grok instances repeatedly violated this by injecting hardcoded
//   "https://www.translate.com", "https://translate.com",  or Github domains.
//   This is **explicitly forbidden**.
//
//   All domain/start-page decisions MUST go through:
//     - START_PAGE constant
//     - should_enable_login_check() if it exists
//     - g_str_has_prefix(uri, START_PAGE)
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Hardcode any URL inside this function (always use START_PAGE from project.h)
//     - Bypass this function for Github/START_PAGE loads
//     - Remove the login handler connection logic
//   Violating the anti-hardcoding rule breaks CIAO Principle 5 and causes maintenance issues.
//
// Last updated: Full CIAO expansion with preserved original comments (2026-05-08)
// ──────────────────────────────────────────────
void load_page_with_login_check(WebKitWebView *web_view, const gchar *uri) {
    if (web_view == NULL || uri == NULL || *uri == '\0') {
        ciao_error("load_page_with_login_check: invalid parameters");
        return;
    }

    // Robust Github detection
    if (g_str_has_prefix(uri, START_PAGE)) {
        ciao_info("[LOGIN-CHECK] Github domain detected → connecting login handler");
        
        // Connect handler (we keep it active for safety across navigations)
        gulong handler_id = g_signal_connect(web_view, "load-changed",
            G_CALLBACK(on_login_page_check), NULL);
        g_object_set_data(G_OBJECT(web_view), "login-check-handler", 
            GUINT_TO_POINTER(handler_id));
    }

    // Always perform the actual load
    load_page(web_view, uri);
}

