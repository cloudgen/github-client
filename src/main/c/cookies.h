// cookies.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - COOKIE PERSISTENCE HEADER (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// !!! DO NOT ADD DIRECT stdio, perror, or low-level system calls HERE !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#ifndef COOKIES_H
#define COOKIES_H

#include <webkit2/webkit2.h>
#include <stddef.h>
#include "project.h"

// =========================================================================
// Public API - Cookie Persistence Module
// 
// Purpose:
//   Provides persistent cookie storage using SQLite for WebKitGTK.
// 
// Design Rules:
//   - All output must go through the central ciao_output system.
//   - Buffer safety is enforced (no blind strcpy/strcat).
//   - Directory creation uses GLib safe functions.
// 
// Protection Rule:
//   Future AI must NOT expose internal helper functions (create_directories, 
//   test_cookie_storage) in the public header.
// =========================================================================

// =========================================================================
// Initialize cookie storage path and ensure directory exists
// 
// Parameters:
//   full_storage_path : Output buffer to receive the full storage path
//   max_len           : Size of the output buffer (recommended: 1024)
// 
// Protection:
//   Always checks for NULL buffers and HOME variable.
// =========================================================================
void initialize_cookie_storage(char *full_storage_path, size_t max_len);

// =========================================================================
// Add persistent cookie support to WebKit context
// 
// Purpose:
//   Configures WebKitWebContext to use persistent SQLite cookie storage.
// 
// Important:
//   Must be called after creating the WebKitWebContext but before loading
//   any web pages.
// =========================================================================
void add_cookie(WebKitWebContext *web_context);

#endif // COOKIES_H