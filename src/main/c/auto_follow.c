// auto_follow.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - AUTO FOLLOW MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Purpose:
//   Provides automatic "Follow" button clicking capability on Github bot pages
//   by injecting and executing JavaScript into the WebKitWebView.
//
// Last aligned with SyncPrjs CIAO style: 2026-05-08
// =========================================================================

#include "auto_follow.h"
#include <string.h>

// ──────────────────────────────────────────────
// AUTO_FOLLOW_SCRIPT - JavaScript Auto-Follow Script
// ──────────────────────────────────────────────
//
// Purpose:
//   Contains the injected JavaScript code that automatically clicks the
//   "Follow" button on Github bot pages with retry logic. This is the
//   Single Point of Truth for the auto-follow behavior.
//
// Parameters:
//   Input: None (static constant)
//   Output/Return: None (string literal used by inject_auto_follow_script)
//
// Dependencies:
//   - Used by: inject_auto_follow_script()
//   - Headers required: auto_follow.h
//   - Relies on: WebKit JavaScript execution environment
//
// Why This Design:
//   Compact single-line JS string to avoid C string escaping issues.
//   Uses retry mechanism (max 20 attempts) because Github pages are heavy SPAs
//   and the Follow button may appear asynchronously.
//
// CIAO Principles Applied:
//   - Caution: Defensive retry logic prevents silent failure
//   - Intentional: Clear purpose for bot pages auto-follow
//   - Anti-fragile: Graceful fallback when button is not found
//   - Over-protect: Core auto-follow logic preserved
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Modify the JavaScript logic without explicit instruction
//     - Convert to multi-line string that breaks escaping
//     - Remove retry mechanism
//   This constant is intentionally kept as-is for stability.
//
// Last updated: Auto-follow enhancement (2026-05-08)
// ──────────────────────────────────────────────
static const char *AUTO_FOLLOW_SCRIPT =
"(() => {"
"  let attempts = 0;"
"  const maxAttempts = 20;"
"  const tryClick = () => {"
"    attempts++;"
"    const button = document.querySelector('button[aria-label=\"Follow\"]');"
"    if (button) {"
"      const ariaLabel = button.getAttribute('aria-label') || '';"
"      button.click();"
"    } else {"
"      if (attempts < maxAttempts) {"
"        setTimeout(tryClick, 500);"
"      }"
"    }"
"  };"
"  tryClick();"
"})();";

// ──────────────────────────────────────────────
// javascript_finished() - JavaScript Evaluation Callback
// ──────────────────────────────────────────────
//
// Purpose:
//   Callback invoked after webkit_web_view_evaluate_javascript() completes.
//   Handles both success and error cases with proper logging.
//
// Parameters:
//   Input:
//     - object     : GObject* - The WebKitWebView that executed the JS
//     - result     : GAsyncResult* - Async result from JavaScript evaluation
//     - user_data  : gpointer - User data (unused in this implementation)
//   Output/Return: None (void callback, logs only)
//
// Dependencies:
//   - Functions called: webkit_web_view_evaluate_javascript_finish()
//   - Headers required: webkit2/webkit2.h, auto_follow.h
//   - External signals: Connected via inject_auto_follow_script()
//
// Why This Design:
//   Required by WebKitGTK async JavaScript API. Centralizes error handling
//   to prevent silent failures during auto-follow injection.
//
// CIAO Principles Applied:
//   - Caution: NULL checks for object and result
//   - Intentional: Clear success/error distinction in logs
//   - Anti-fragile: Graceful error reporting without crashing
//   - Over-protect: Defensive callback pattern
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Remove NULL checks
//     - Remove error handling or g_error_free()
//     - Change to silent failure
//   This callback is critical for debugging JS injection issues.
//
// Last updated: Defensive callback hardening (2026-05-08)
// ──────────────────────────────────────────────
static void javascript_finished(GObject *object, GAsyncResult *result, gpointer user_data)
{
    if (object == NULL) {
        g_print("[AUTO_FOLLOW] ERROR: object is NULL in callback\n");
        return;
    }

    if (result == NULL) {
        g_print("[AUTO_FOLLOW] ERROR: result is NULL in callback\n");
        return;
    }
    GError *error = NULL;
    webkit_web_view_evaluate_javascript_finish(WEBKIT_WEB_VIEW(object), result, &error);

    if (error != NULL) {
        g_print("[AUTO_FOLLOW] JavaScript ERROR: %s\n", error->message);
        g_error_free(error);
    } else {
        g_print("[AUTO_FOLLOW] JavaScript executed successfully (no error reported)\n");
    }
}

// ──────────────────────────────────────────────
// inject_auto_follow_script() - Inject Auto-Follow JavaScript
// ──────────────────────────────────────────────
//
// Purpose:
//   Single Point of Truth for injecting the auto-follow script into a
//   WebKitWebView. Used for bot pages that require automatic "Follow" clicks.
//
// Parameters:
//   Input:
//     - web_view : WebKitWebView* - Target web view (must not be NULL)
//   Output/Return: None (void function)
//
// Dependencies:
//   - Functions called: javascript_finished() (callback)
//   - Headers required: auto_follow.h, webkit2/webkit2.h
//   - External signals: None (direct injection)
//
// Why This Design:
//   Multiple defensive checks before injection to prevent crashes.
//   Uses async JavaScript evaluation with dedicated callback for reliability.
//
// CIAO Principles Applied:
//   - Caution: Full NULL and type validation before injection
//   - Intentional: Clear purpose for bot auto-follow feature
//   - Anti-fragile: Safe no-op on invalid web_view
//   - Over-protect: Core auto-follow injection logic
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Remove any of the three defensive checks
//     - Bypass this function for auto-follow
//     - Simplify or inline the injection logic
//   This is the only authorized entry point for auto-follow script.
//
// Last updated: Full CIAO comment expansion (2026-05-08)
// ──────────────────────────────────────────────
void inject_auto_follow_script(WebKitWebView *web_view)
{
    if (web_view == NULL) {
        return;
    }
    if (!G_IS_OBJECT(web_view)) {
        return;
    }
    if (!WEBKIT_IS_WEB_VIEW(web_view)) {
        return;
    }
    webkit_web_view_evaluate_javascript(
        web_view,
        AUTO_FOLLOW_SCRIPT,
        -1,
        NULL,
        NULL,
        NULL,
        javascript_finished,
        NULL
    );
}
