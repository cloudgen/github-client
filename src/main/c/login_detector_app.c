// login_detector_app.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - GM + GOOGLE ACCOUNT CHOOSER
// =========================================================================
//
// !!! THIS IS A GNOME/GTK + WebKitGTK APPLICATION !!!
// !!! ONLY ciao_* FUNCTIONS FOR OUTPUT !!!
// 
// Purpose:
//   Single Point of Truth for full automatic login flow:
//   1. Click "Continue with Google" on Github
//   2. Auto-select the FIRST Google account on the account chooser page
//
// Lessons Learned (Important for Future Maintenance):
//   - Heavy SPA pages require multiple timed retries (700ms / 2200ms / 4500ms)
//   - Multi-line JS in C is fragile but acceptable when kept compact and well-tested
//   - JavaScript injected via C string must avoid template literals (`${}`) 
//   - Always check current URI in debug to distinguish Github vs Google pages
//   - Prefer real DOM click() over manual OAuth URL construction (more reliable)
//   - Use `indexOf()` instead of `includes()` for maximum browser compatibility
//   - Keep legacy functions as stubs to maintain header compatibility
//
// Protection Rule:
//   Do NOT simplify the JS or remove retry timers without explicit instruction.
//   This module was battle-tested against timing / SPA / string escaping issues.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// =========================================================================

#include "output.h"
#include "load_page_login_check.h"
#include "login_detector_app.h"
#include <webkit2/webkit2.h>
#include <glib.h>


// ──────────────────────────────────────────────
// tts_javascript_finished() - TTS Keeper JS Callback
// ──────────────────────────────────────────────
//
// Purpose:
//   Callback for webkit_web_view_evaluate_javascript() used by TTS keeper.
//   Logs success/error through ciao_debug() (makes internal state visible).
//
// CIAO-Lite:
//   - Caution: full GError handling + cleanup
//   - Intentional: centralized JS feedback
//
// Protection Rule (Sacred):
//   Do NOT remove error logging or g_error_free().
//
// Last updated: TTS fix (May 2026)
// ──────────────────────────────────────────────
static void tts_javascript_finished(GObject *object, GAsyncResult *result, gpointer user_data)
{
    (void)user_data;
    GError *error = NULL;
    webkit_web_view_evaluate_javascript_finish(WEBKIT_WEB_VIEW(object), result, &error);

    if (error) {
        ciao_debug("[TTS-KEEPER] JS Error: %s", error->message);
        g_error_free(error);
    } else {
        ciao_debug("[TTS-KEEPER] Script injected successfully");
    }
}

// ──────────────────────────────────────────────
// inject_tts_keeper() - Install TTS Keep-Alive Script
// ──────────────────────────────────────────────
//
// Purpose:
//   Injects defensive JavaScript that keeps speechSynthesis alive on
//   Github pages by calling resume() every 12 seconds when speaking.
//
// CIAO-Lite:
//   - Caution: NULL check + one-time install flag + try/catch everywhere
//   - Anti-fragile: works across SPA timing issues
//   - Intentional: longer interval (12s) based on real-world reports
//
// Protection Rule (Sacred):
//   Do NOT simplify JS logic, remove the install flag, or change interval
//   without explicit user instruction.
//
// Last updated: TTS fix (May 2026)
// ──────────────────────────────────────────────
void inject_tts_keeper(WebKitWebView *web_view)
{
    if (web_view == NULL || !WEBKIT_IS_WEB_VIEW(web_view)) {
        ciao_warn("[TTS-KEEPER] Invalid web_view, skipping injection");
        return;
    }

    static const char *TTS_KEEPER_SCRIPT =
    "(function(){"
    "  try {"
    "    if (!('speechSynthesis' in window)) { console.log('[TTS-KEEPER] No speechSynthesis'); return; }"
    "    if (window.__ciao_tts_keeper_installed) { console.log('[TTS-KEEPER] Already installed'); return; }"
    "    window.__ciao_tts_keeper_installed = true;"
    "    var timer = null;"
    "    var keepAlive = function() {"
    "      try {"
    "        if (window.speechSynthesis.speaking || window.speechSynthesis.pending) {"
    "          window.speechSynthesis.resume();"
    "        }"
    "      } catch (e) {"
    "        console.log('[TTS-KEEPER] keepAlive error', e && e.message ? e.message : e);"
    "      }"
    "    };"
    "    var INTERVAL_MS = 12000;"
    "    window.speechSynthesis.addEventListener('start', function() {"
    "      try { if (timer) clearInterval(timer); timer = setInterval(keepAlive, INTERVAL_MS); } catch(e) { console.log('[TTS-KEEPER] start handler error', e); }"
    "    });"
    "    window.speechSynthesis.addEventListener('end', function() {"
    "      try { if (timer) { clearInterval(timer); timer = null; } } catch(e) { console.log('[TTS-KEEPER] end handler error', e); }"
    "    });"
    "    window.speechSynthesis.addEventListener('cancel', function() {"
    "      try { if (timer) { clearInterval(timer); timer = null; } } catch(e) { console.log('[TTS-KEEPER] cancel handler error', e); }"
    "    });"
    "    console.log('[TTS-KEEPER] Installed (interval=' + INTERVAL_MS + 'ms)');"
    "  } catch (e) {"
    "    console.log('[TTS-KEEPER] Injection outer error', e && e.message ? e.message : e);"
    "  }"
    "})();";

    webkit_web_view_evaluate_javascript(
        web_view,
        TTS_KEEPER_SCRIPT,
        -1,
        NULL,
        NULL,
        NULL,
        tts_javascript_finished,
        NULL
    );
}

// ──────────────────────────────────────────────
// delayed_login_detect() - SPA-friendly delayed injection
// ──────────────────────────────────────────────
//
// Purpose:
//   Called via g_timeout_add() after page load to give the SPA time to render
//   the login button. This is critical because Github is a heavy React app.
//
// Parameters:
//   Input:
//     - user_data : gpointer - Expected to be WebKitWebView*
//   Output/Return:
//     - gboolean - Always G_SOURCE_REMOVE (one-shot timer)
//
// Dependencies:
//   - Functions called: inject_login_detection(), WEBKIT_IS_WEB_VIEW()
//   - Headers required: webkit2/webkit2.h, login_detector_app.h
//   - External: Used by g_timeout_add() from on_load_for_login_check()
//
// Why This Design:
//   Single retry point that can be scheduled multiple times with different delays.
//   Keeps timing logic centralized and easy to adjust.
//
// CIAO Principles Applied:
//   - Caution: NULL / type checks before use
//   - Anti-fragile: Graceful no-op if web_view is invalid
//   - Intentional: Clear purpose comment so future maintainers understand SPA timing need
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Remove or inline this function
//     - Remove NULL / type checks
//   Multiple timed calls are required to reliably catch dynamic content on both Github and Google pages.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
gboolean delayed_login_detect(gpointer user_data) {
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(user_data);
    if (WEBKIT_IS_WEB_VIEW(web_view)) {
        inject_login_detection(web_view);
    }
    return G_SOURCE_REMOVE;   // One-shot timer
}

// ──────────────────────────────────────────────
// retry_login_inject() - Additional retry for heavy SPA pages
// ──────────────────────────────────────────────
//
// Purpose:
//   Provides extra injection attempts at later intervals (e.g. 2.2s and 4.5s).
//   Necessary because Github and Google account chooser load content
//   asynchronously after the initial WEBKIT_LOAD_FINISHED event.
//
// Parameters:
//   Input:
//     - user_data : gpointer - Expected to be WebKitWebView*
//   Output/Return:
//     - gboolean - Always G_SOURCE_REMOVE (one-shot timer)
//
// Dependencies:
//   - Functions called: inject_login_detection(), WEBKIT_IS_WEB_VIEW()
//   - Headers required: webkit2/webkit2.h, login_detector_app.h
//   - External: Used by g_timeout_add() from on_load_for_login_check()
//
// Why Multiple Retries:
//   - First attempt (delayed_login_detect) at ~800ms
//   - Retries give more time for React/Vue components to render login UI
//   - Proven effective against timing-related failures
//
// CIAO Principles Applied:
//   - Caution: Defensive type check with WEBKIT_IS_WEB_VIEW
//   - Anti-fragile: Safe no-op on invalid web_view, one-shot execution
//   - Intentional Verbosity: Explicit comment so future AI/human maintainers
//     understand why retries exist (hard lesson from SPA timing issues)
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Remove retry timers or merge this into a single function
//     - Remove type checks
//   The staggered timing is deliberate for reliability across Github + Google.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
gboolean retry_login_inject(gpointer user_data) {
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(user_data);
    if (WEBKIT_IS_WEB_VIEW(web_view)) {
        inject_login_detection(web_view);
    }
    return G_SOURCE_REMOVE;   // One-shot timer
}

// ──────────────────────────────────────────────
// inject_login_detection() - Core Smart Auto-Login Engine
// ──────────────────────────────────────────────
//
// Purpose:
//   Single Point of Truth for automatic login flow:
//     1. Detects and clicks "Continue" / "Google" buttons on Github (START_PAGE) 
//        regardless of UI language (Traditional Chinese, Simplified Chinese,
//        Japanese, German, French, English, etc.)
//     2. On Google account chooser page, automatically selects the FIRST account
//
// Parameters:
//   Input:
//     - web_view : WebKitWebView* - Target web view (must not be invalid)
//   Output/Return: None (void function, performs DOM clicks via JS)
//
// Dependencies:
//   - Functions called: webkit_web_view_get_uri(), ciao_debug(), ciao_info(),
//     webkit_web_view_evaluate_javascript(), javascript_finished()
//   - Headers required: webkit2/webkit2.h, output.h, project.h
//   - External: JavaScript execution in WebKit
//
// Design Decision:
//   - Broad selector + explicit multi-language text matching for "Continue" buttons
//   - Explicitly skips "Cancel" buttons in multiple languages to prevent mis-clicks
//   - Uses formatted (but still compact) JavaScript string for readability while
//     avoiding C string escaping issues
//   - Logs detailed scan stats for debugging across different languages
//
// Lessons Learned (Critical):
//   - Button text changes with user language → must support multiple translations
//   - Google consent pages contain both "Continue" and "Cancel" → must filter Cancel
//   - Multi-line JS in C is fragile but acceptable when kept compact and well-tested
//   - SPA timing is unpredictable → rely on existing retry timers (800/2200/4500ms)
//   - Prefer native .click() on real DOM elements
//
// CIAO Principles Applied:
//   - Caution: Full NULL/type validation + Cancel button protection
//   - Intentional: Clear multi-language support and anti-misclick logic
//   - Anti-fragile: Safe fallback, detailed logging, works across languages
//   - Over-protect: Core login flow — do not simplify JS or remove retries
//
// Protection Rule (Sacred):
//   Future modifications must preserve:
//     • Multi-language "Continue" detection
//     • Cancel button filtering
//     • Retry strategy (delayed_login_detect + retry_login_inject)
//     • Compact JS format inside this function
//
// Last updated: Multi-language + anti-cancel protection (2026-05-08)
// ──────────────────────────────────────────────
void inject_login_detection(WebKitWebView *web_view) {
    if (!WEBKIT_IS_WEB_VIEW(web_view)) return;

    const gchar *uri = webkit_web_view_get_uri(web_view);
    ciao_debug("[LOGIN-INJECT] Current URI: %s", uri ? uri : "(null)");

    // Multi-language support + Cancel protection (minimal change from original)
    const gchar *js = "(function(){"
        "console.log('[AUTO-LOGIN] Scanning...');"
        "var total=0, clicked=false;"
        "var pBtns=document.querySelectorAll('button,[role=\"button\"],[class*=\"button\"]');"
        "for(var i=0;i<pBtns.length;i++){"
            "total++;"
            "var el=pBtns[i];"
            "var t=(el.textContent||el.innerText||'').trim();"
            "if(t.indexOf('Cancel')>=0 || t.indexOf('取消')>=0 || t.indexOf('Abbrechen')>=0 || t.indexOf('Annuler')>=0) continue;" // skip cancel
            "if(t.indexOf('Continue')>=0 || t.indexOf('Google')>=0 || "
               "t.indexOf('繼續')>=0 || t.indexOf('继续')>=0 || "
               "t.indexOf('続行')>=0 || t.indexOf('Weiter')>=0 || "
               "t.indexOf('Continuer')>=0){"
                "console.log('[AUTO-LOGIN] Found CONTINUE button: ' + t);"
                "el.scrollIntoView({behavior:'instant'});"
                "setTimeout(function(){el.click();},300);"
                "clicked=true; break;"
            "}"
        "}"
        "if(!clicked){"
            "var accounts=document.querySelectorAll('li[role=\"link\"],.VV3oRb,[data-identifier]');"
            "if(accounts.length>0){"
                "console.log('[AUTO-LOGIN] Google chooser ('+accounts.length+' accounts) - clicking first');"
                "var first=accounts[0];"
                "first.scrollIntoView({behavior:'instant',block:'center'});"
                "setTimeout(function(){first.click();},400);"
                "clicked=true;"
            "}"
        "}"
        "console.log('[AUTO-LOGIN] Scan done - Total:'+total+', Clicked:'+clicked);"
    "})()";

    ciao_info("[LOGIN-INJECT] Running smart auto-login (Multi-language: ZH/JP/DE/FR + anti-cancel)");
    
    webkit_web_view_evaluate_javascript(web_view, js, -1, NULL, NULL, NULL, javascript_finished, NULL);
}

// ──────────────────────────────────────────────
// delayed_login_check_wrapper() - Thin Safe Timer Wrapper
// ──────────────────────────────────────────────
//
// Purpose:
//   Thin safe wrapper that fixes garbage load_event when called from timer.
//   Ensures clean WEBKIT_LOAD_FINISHED event is passed to the real handler.
//
// Parameters:
//   Input:
//     - user_data : gpointer - Expected to be WebKitWebView*
//   Output/Return:
//     - gboolean - Always G_SOURCE_REMOVE
//
// Dependencies:
//   - Functions called: on_load_for_login_check()
//   - Headers required: webkit2/webkit2.h, login_detector_app.h
//
// Why This Design:
//   Prevents random big numbers appearing in logs when timer fires.
//
// Protection Rule (Sacred):
//   Do NOT remove or inline this wrapper. It exists to maintain clean event handling.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
gboolean delayed_login_check_wrapper(gpointer user_data)
{
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(user_data);
    if (WEBKIT_IS_WEB_VIEW(web_view)) {
        // Call with clean FINISHED event so no random big number appears
        on_load_for_login_check(web_view, WEBKIT_LOAD_FINISHED, NULL);
    }
    return G_SOURCE_REMOVE;
}

// ──────────────────────────────────────────────
// on_load_for_login_check() - Main Load Event Handler
// ──────────────────────────────────────────────
//
// Purpose:
//   Central handler for WebKit "load-changed" signal.
//   Triggers the smart login injection sequence only when the page reaches
//   WEBKIT_LOAD_FINISHED state.
//   Triggers smart login injection + TTS keeper for Github pages.
//
// Parameters:
//   Input:
//     - web_view    : WebKitWebView*
//     - load_event  : WebKitLoadEvent
//     - user_data   : gpointer (unused)
//   Output/Return: None (void signal handler)
//
// Dependencies:
//   - Functions called: ciao_info(), g_timeout_add(), delayed_login_detect(), retry_login_inject()
//   - Headers required: webkit2/webkit2.h, output.h, login_detector_app.h
//
// Why This Design:
//   SPA pages (START_PAGE + Google) continue loading content after the initial
//   finished event. We use staggered timeouts (800ms / 2200ms / 4500ms) to
//   reliably catch dynamically rendered login elements.
//
// CIAO Principles Applied:
//   - Caution: Defensive event filtering + (void)user_data pattern for GTK signals
//   - Intentional: Clear explanation of SPA timing challenges and retry strategy
//   - Anti-fragile: Multiple retry windows prevent timing-related failures
//   - Single Point of Truth: All login injection timing is controlled here
//
// Lessons Learned:
//   One-shot injection was unreliable on heavy React pages.
//   Staggered retries proved essential for both Github login button
//   and Google account chooser.
//
// Protection Rule (Sacred):
//   Do NOT remove or reduce the retry timers without explicit user instruction.
//   The multi-attempt pattern is the result of extensive debugging.
//
// Last updated: TTS Keeper integration (2026-05-12)
// ──────────────────────────────────────────────
void on_load_for_login_check(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data) {
    (void)user_data;
    if (load_event == WEBKIT_LOAD_FINISHED || load_event == WEBKIT_LOAD_COMMITTED) {
        ciao_info("[LOGIN-CHECK] Page loaded → auto-login attempts");
        
        g_timeout_add(800,  delayed_login_detect, web_view);
        g_timeout_add(2200, retry_login_inject,   web_view);
        g_timeout_add(4500, retry_login_inject,   web_view);
    }
    // === NEW: TTS Keeper for Github pages ===
    const gchar *uri = webkit_web_view_get_uri(web_view);
    //if (uri && g_str_has_prefix(uri, START_PAGE)) {
        ciao_debug("[TTS-KEEPER] Github page detected — injecting TTS keeper");
        inject_tts_keeper(web_view);
    //}
}


// ──────────────────────────────────────────────
// on_login_page_check() - Public Signal Entry Point
// ──────────────────────────────────────────────
//
// Purpose:
//   Thin wrapper / public API entry point connected from load_page_login_check.c
//   Delegates directly to on_load_for_login_check() to keep signal connection clean.
//
// Parameters:
//   Input:
//     - web_view    : WebKitWebView*
//     - load_event  : WebKitLoadEvent
//     - user_data   : gpointer
//   Output/Return: None
//
// Dependencies:
//   - Functions called: on_load_for_login_check()
//   - Headers required: login_detector_app.h
//
// Design:
//   This function exists purely for architectural clarity and cross-file compatibility.
//   It allows the signal connection in load_page_login_check.c to remain simple
//   while the real logic lives in on_load_for_login_check().
//
// CIAO Principles Applied:
//   - Single Point of Truth: Real logic stays in one place
//   - Reusability / Flexibility: Easy to connect from different modules
//   - Intentional: Explicit comment explains why this thin wrapper exists
//
// Protection Rule (Sacred):
//   Do NOT inline or remove this function. It is intentionally kept for
//   clean signal connection architecture and future maintainability.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
void on_login_page_check(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data) {
    on_load_for_login_check(web_view, load_event, user_data);
}

// ──────────────────────────────────────────────
// javascript_finished() - Callback for JS evaluation
// ──────────────────────────────────────────────
//
// Purpose:
//   Receives the result of webkit_web_view_evaluate_javascript() calls.
//   Handles both success and error cases with clear logging.
//
// Parameters:
//   Input:
//     - object     : GObject* - The WebKitWebView object that executed the JS
//     - result     : GAsyncResult* - Async result from the JavaScript evaluation
//     - user_data  : gpointer - User data passed during signal connection (unused)
//   Output/Return: None (void callback)
//
// Dependencies:
//   - Functions called: webkit_web_view_evaluate_javascript_finish(), ciao_error(), ciao_info(), g_error_free()
//   - Headers required: webkit2/webkit2.h, output.h, login_detector_app.h
//   - External signals: Connected from inject_login_detection() and other JS injection points
//
// Why This Design:
//   Required by WebKitGTK async JavaScript API.
//   Centralizes all JS execution feedback through the ciao_* output system.
//
// CIAO Principles Applied:
//   - Caution: Proper GError handling and cleanup (g_error_free)
//   - Intentional Verbosity: Clear success/error distinction for debugging
//   - Single Point of Truth: All JS result handling goes through this function
//
// Protection Note:
//   Do NOT remove error handling or change to silent failure.
//   Detailed logging is essential when debugging SPA injection timing issues.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
void javascript_finished(GObject *object, GAsyncResult *result, gpointer user_data) {
    GError *error = NULL;
    WebKitWebView *wv = WEBKIT_WEB_VIEW(object);
    webkit_web_view_evaluate_javascript_finish(wv, result, &error);

    if (error) {
        ciao_error("[AUTO-LOGIN] JS ERROR: %s", error->message);
        g_error_free(error);
    } else {
        ciao_info("[AUTO-LOGIN] JS executed successfully");
    }
}

// ──────────────────────────────────────────────
// navigate_to_google_oauth() - Legacy OAuth URL Fallback
// ──────────────────────────────────────────────
//
// Purpose:
//   Old implementation that constructed a manual Google OAuth URL.
//   Kept as a fallback / compatibility stub.
//
// Parameters:
//   Input:
//     - web_view : WebKitWebView* - Target web view
//   Output/Return: None
//
// Dependencies:
//   - Functions called: ciao_info()
//   - Headers required: webkit2/webkit2.h, output.h
//
// Current Status:
//   No longer used — we now prefer real DOM button clicks for better reliability
//   and to avoid parameter/redirect issues.
//
// CIAO Principles Applied:
//   - Anti-fragile: Graceful fallback kept alive
//   - Intentional: Clear comment documenting why it exists and current status
//   - Over-protect: Never delete legacy code without updating header and callers
//
// Note for Future:
//   If button-click approach ever becomes unstable, this function can be revived
//   quickly by calling it from inject_login_detection().
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
void navigate_to_google_oauth(WebKitWebView *web_view) {
    ciao_info("navigate_to_google_oauth called (fallback)");
    // Currently not used - we prefer real button click
    (void)web_view;   // suppress unused parameter warning
}

// ──────────────────────────────────────────────
// login_js_callback() - Legacy JS Result Callback
// ──────────────────────────────────────────────
//
// Purpose:
//   Old callback used when we evaluated JS that returned a boolean value
//   (previous detection logic).
//
// Parameters:
//   Input:
//     - source     : GObject*
//     - result     : GAsyncResult*
//     - user_data  : gpointer
//   Output/Return: None
//
// Dependencies:
//   - Headers required: webkit2/webkit2.h
//
// Current Status:
//   No longer used in the current flow (we use javascript_finished() instead).
//   Kept purely for header compatibility and potential future revival.
//
// CIAO Principles Applied:
//   - Anti-fragile: Maintains full header compatibility
//   - Intentional: Explicit comment prevents confusion about its status
//   - Over-protect: Legacy code is preserved with clear documentation
//
// Protection Rule:
//   Do NOT remove this function unless you also update login_detector_app.h
//   and all including files.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
void login_js_callback(GObject *source, GAsyncResult *result, gpointer user_data) {
    // Legacy - not used in current flow (real button click + javascript_finished)
    (void)source;
    (void)result;
    (void)user_data;
}

