// login_detector_app.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - GM LOGIN DETECTION MODULE
// =========================================================================
//
// !!! THIS IS A GNOME/GTK + WebKitGTK APPLICATION !!!
// !!! ONLY g_print()/g_printerr() THROUGH ciao_* FUNCTIONS !!!
// 
// Purpose: Detects Github login modal and auto-clicks "Continue with Google"
// 
// Last aligned with CIAO style + SPA retry logic: 2026-05-02
// =========================================================================

#ifndef LOGIN_DETECTOR_GITHUB_H
#define LOGIN_DETECTOR_GITHUB_H

#include <webkit2/webkit2.h>
#include "project.h"

// PUBLIC API
gboolean is_app_login_url(const char *uri);

gboolean delayed_login_inject(gpointer user_data);
gboolean retry_login_inject(gpointer user_data);          // ← NEW: retry for SPA

// Start Page Google auto-click (used by url_bar_page.c)
gboolean delayed_startpage_google(gpointer user_data);
gboolean delayed_login_check_wrapper(gpointer user_data);

void javascript_finished(GObject *object, GAsyncResult *result, gpointer user_data);
void login_js_callback(GObject *source, GAsyncResult *result, gpointer user_data);

void inject_login_detection(WebKitWebView *web_view);

void on_load_for_login_check(WebKitWebView *web_view,
                             WebKitLoadEvent load_event,
                             gpointer user_data);

void on_login_page_check(WebKitWebView *web_view,
                         WebKitLoadEvent load_event,
                         gpointer user_data);

void navigate_to_google_oauth(WebKitWebView *web_view);

#endif // LOGIN_DETECTOR_GITHUB_H