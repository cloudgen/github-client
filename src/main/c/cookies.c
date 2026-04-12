// cookies.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - COOKIE PERSISTENCE MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED !!!
// !!! ALL OUTPUT MUST GO THROUGH ciao_* FUNCTIONS !!!
// !!! DO NOT USE printf(), fprintf(), perror(), mkdir(), etc. !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#include "cookies.h"
#include "output.h"
#include "project.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>

// =========================================================================
// General Purpose Requirement: Cookie Storage Directory Creation
// 
// Purpose:
//   Creates the full directory tree for persistent cookie storage safely.
// 
// Why This Design:
//   Uses GLib's g_mkdir_with_parents() for reliability and portability.
//   All errors are reported through the central output system.
// 
// Protection Rule:
//   Future AI must NOT replace this with manual strtok/mkdir loops.
//   This function defends against path traversal and permission issues.
// =========================================================================
static void ciao_create_storage_directories(const char *path)
{
    if (path == NULL || *path == '\0') {
        ciao_error("Cannot create directories: path is empty");
        return;
    }

    gint result = g_mkdir_with_parents(path, 0700);
    if (result == -1) {
        ciao_error("Failed to create directory tree: %s", path);
        // Do not use perror() - GNOME compliance
    } else {
        ciao_debug("Cookie storage directories ensured: %s", path);
    }
}

// =========================================================================
// General Purpose Requirement: Initialize Cookie Storage
// 
// Purpose:
//   Builds the full path to the persistent cookie storage and ensures
//   the directory structure exists.
// 
// Single Point of Entry for cookie storage initialization.
// =========================================================================
void initialize_cookie_storage(char *full_storage_path, size_t max_len)
{
    if (full_storage_path == NULL || max_len == 0) {
        ciao_error("initialize_cookie_storage: invalid buffer");
        return;
    }

    const char *home = getenv("HOME");
    if (home == NULL || *home == '\0') {
        ciao_error("HOME environment variable is not set");
        g_strlcpy(full_storage_path, "", max_len);
        return;
    }

    gchar *storage_path = g_build_filename(home, PERSISTENCE_STORAGE_PATH, NULL);
    
    if (storage_path == NULL) {
        ciao_error("Failed to build cookie storage path");
        g_strlcpy(full_storage_path, "", max_len);
        return;
    }

    g_strlcpy(full_storage_path, storage_path, max_len);
    ciao_create_storage_directories(storage_path);

    g_free(storage_path);

    ciao_debug("Cookie storage initialized at: %s", full_storage_path);
}

// =========================================================================
// General Purpose Requirement: Add Persistent Cookie Support
// 
// Purpose:
//   Configures WebKitWebContext to use persistent SQLite cookie storage.
// 
// Protection Rule:
//   Must be called after WebKitWebContext is created but before loading content.
// =========================================================================
void add_cookie(WebKitWebContext *web_context)
{
    if (web_context == NULL) {
        ciao_error("add_cookie: web_context is NULL");
        return;
    }

    char full_storage_path[1024] = {0};
    char cookie_path[1039] = {0};

    initialize_cookie_storage(full_storage_path, sizeof(full_storage_path));

    if (full_storage_path[0] == '\0') {
        ciao_error("Failed to initialize cookie storage path");
        return;
    }

    g_snprintf(cookie_path, sizeof(cookie_path), "%s/cookies.sqlite", full_storage_path);

    WebKitCookieManager *cookie_manager = webkit_web_context_get_cookie_manager(web_context);

    if (cookie_manager == NULL) {
        ciao_error("Failed to get WebKitCookieManager");
        return;
    }

    webkit_cookie_manager_set_persistent_storage(
        cookie_manager,
        cookie_path,
        WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE
    );

    ciao_info("Persistent cookie storage enabled: %s", cookie_path);
}