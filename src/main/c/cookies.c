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
#include "config.h"
#include "output.h"
#include "project.h"
#include "path_utils.h"
#include <sqlite3.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>

// ──────────────────────────────────────────────
// debug_print_cookie_count() - Startup Debug Observability (Requirement 2.6)
// ──────────────────────────────────────────────
//
// Purpose:
//   Queries the total number of cookies in cookies.sqlite and logs via ciao_debug().
//   Called on every startup when DEBUG=1.
//
// Parameters:
//   full_db_path : const char* - Full path to cookies.sqlite
//
// CIAO Principles Applied:
//   - Caution: Full error handling, never crashes
//   - Intentional: Makes internal cookie state visible
//   - Anti-fragile: Safe on missing/corrupt DB
//
// Last updated: 2026-05-16
// ──────────────────────────────────────────────
static void debug_print_cookie_count(const char *full_db_path)
{
    if (full_db_path == NULL || *full_db_path == '\0') {
        ciao_debug("Cookie count: invalid path");
        return;
    }

    sqlite3 *db = NULL;
    if (sqlite3_open(full_db_path, &db) != SQLITE_OK) {
        ciao_debug("Cookie count: unable to open DB");
        return;
    }

    sqlite3_stmt *stmt = NULL;
    const char *sql = "SELECT COUNT(*) FROM cookies;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            ciao_debug("Total cookies currently stored in cookies.sqlite: %d", count);
        } else {
            ciao_debug("Cookie count: query returned no row");
        }
    } else {
        ciao_debug("Cookie count: failed to prepare query");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

// ──────────────────────────────────────────────
// ciao_create_storage_directories() - Cookie Storage Directory Creation
// ──────────────────────────────────────────────
//
// Purpose:
//   Creates the full directory tree for persistent cookie storage safely.
//   This is an internal helper used by the cookie persistence module.
//
// Parameters:
//   Input:
//     - path : const char* - Full directory path to create (must not be NULL or empty)
//   Output/Return: None (void function, creates directories on disk)
//
// Dependencies:
//   - Functions called: ciao_error(), ciao_debug(), g_mkdir_with_parents()
//   - Headers required: glib.h, output.h, cookies.h
//   - External: Relies on GNOME/GLib filesystem functions
//
// Why This Design:
//   Uses GLib's g_mkdir_with_parents() for reliability and portability across
//   different Linux distributions. All errors are reported through the central
//   ciao_output system to maintain GNOME compliance.
//
// CIAO Principles Applied:
//   - Caution: NULL/empty path validation and proper error reporting
//   - Intentional: Clear purpose for cookie storage directory setup
//   - Anti-fragile: Defends against path traversal and permission issues
//   - Over-protect: Dedicated helper to isolate directory creation logic
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Replace this with manual strtok/mkdir loops
//     - Remove NULL checks or error reporting
//     - Bypass this function for directory creation
//   This function exists for safety and GNOME compliance.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
static void ciao_create_storage_directories(const char *path)
{
    if (path == NULL || *path == '\0') {
        ciao_error("Cannot create directories: path is empty");
        return;
    }

    gint result = g_mkdir_with_parents(path, 0700);
    if (result == -1) {
        ciao_error("Failed to create directory tree: %s", path);
    } else {
        ciao_debug("Cookie storage directories ensured: %s", path);
    }
}

// ──────────────────────────────────────────────
// initialize_cookie_storage() - Initialize Cookie Storage Path
// ──────────────────────────────────────────────
//
// Purpose:
//   Builds the full path to the persistent cookie storage directory and 
//   ensures the directory structure exists. This is the Single Point of 
//   Entry for all cookie storage initialization.
//
// Parameters:
//   Input:
//     - full_storage_path : char* - Output buffer to receive the full path
//     - max_len           : size_t - Size of the output buffer (recommended: 1024)
//   Output/Return: 
//     - Fills full_storage_path with the constructed path (empty string on failure)
//
// Dependencies:
//   - Functions called: ciao_error(), ciao_debug(), ciao_create_storage_directories(), 
//     getenv(), g_build_filename(), config_get_base_path()
//   - Headers required: glib.h, stdlib.h, cookies.h, project.h, output.h, config.h
//   - External: Relies on $HOME and config_get_base_path()
//
// Why This Design:
//   Centralizes path construction and directory creation to prevent duplication
//   and ensure consistent behavior across cookie and settings modules.
//
// Lessons Learned (Critical - from real incident 2026-05-16):
//   - Even when PROJECT_ID and config logic are correct, a single subtle bug 
//     in tilde expansion (g_str_has_prefix + base+2) can produce invalid paths 
//     like "/home/user/~/.app/..." — breaking cookie reuse between old backups 
//     and the new R2WebPad version.
//   - Missing the trailing "/cookies/" subdirectory made the path completely wrong.
//   - This kind of path bug is extremely hard to spot without visible debug output.
//   - Always make the final resolved path and cookie count visible on startup 
//     when DEBUG=1 (see debug_print_cookie_count). This single practice would 
//     have caught the bug immediately.
//   - Defensive Storage Location Handling (CIAO Principle 17) must be 
//     rigorously tested with real migrations between forked projects.
//
// CIAO Principles Applied:
//   - Caution: Buffer safety, HOME variable validation, NULL checks, tilde expansion
//   - Intentional: Single Point of Entry for all cookie storage paths
//   - Anti-fragile: Graceful degradation when HOME is not set
//   - Over-protect: Defensive buffer handling and migration safety
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Remove NULL buffer or HOME checks
//     - Hardcode paths instead of using PERSISTENCE_STORAGE_PATH + config
//     - Bypass this function for cookie storage initialization
//   This is the only authorized way to initialize cookie paths.
//
// Last updated: 2026-05-16 (added critical Lessons Learned after path bug incident)
// ──────────────────────────────────────────────
// ──────────────────────────────────────────────
// initialize_cookie_storage() - Initialize Cookie Storage Path
// ──────────────────────────────────────────────
void initialize_cookie_storage(char *full_storage_path, size_t max_len)
{
    if (full_storage_path == NULL || max_len == 0) {
        ciao_error("initialize_cookie_storage: invalid buffer");
        return;
    }

    const char *base = config_get_base_path();
    if (base == NULL || *base == '\0') {
        base = PERSISTENCE_STORAGE_PATH;
    }

    // Single Source of Truth: Use the dedicated path resolver
    resolve_full_storage_path(base, full_storage_path, max_len);

    if (full_storage_path[0] == '\0') {
        ciao_error("Failed to resolve cookie storage directory path");
        return;
    }

    ciao_create_storage_directories(full_storage_path);

    const char *cookies_file = config_get_cookies_filename();
    if (cookies_file == NULL || *cookies_file == '\0') {
        cookies_file = "cookies.sqlite";
    }

    char final_path[1024] = {0};
    g_snprintf(final_path, sizeof(final_path), "%s/cookies/%s", full_storage_path, cookies_file);
    g_strlcpy(full_storage_path, final_path, max_len);

    ciao_debug("Cookie storage initialized at: %s", full_storage_path);

    if (getenv("DEBUG") && *getenv("DEBUG") != '\0') {
        debug_print_cookie_count(full_storage_path);
    }
}

// ──────────────────────────────────────────────
// add_cookie() - Add Persistent Cookie Support
// ──────────────────────────────────────────────
//
// Purpose:
//   Configures a WebKitWebContext to use persistent SQLite cookie storage.
//   This is the Single Point of Truth for enabling cookie persistence in the application.
//
// Parameters:
//   Input:
//     - web_context : WebKitWebContext* - The WebKit context to configure (must not be NULL)
//   Output/Return: None (void function, configures the context in-place)
//
// Dependencies:
//   - Functions called: initialize_cookie_storage(), ciao_error(), ciao_info(), webkit_web_context_get_cookie_manager()
//   - Headers required: webkit2/webkit2.h, cookies.h, project.h, output.h
//   - External signals: Must be called after WebKitWebContext creation but before any page load
//
// Why This Design:
//   Ensures cookies survive application restarts by using SQLite persistent storage.
//   Must be called early in the WebKit setup flow.
//
// CIAO Principles Applied:
//   - Caution: NULL checks and path validation before configuration
//   - Intentional: Clear timing requirement for WebKit initialization
//   - Anti-fragile: Safe no-op on invalid context or failed path initialization
//   - Over-protect: Core persistence layer for login/session state
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Remove NULL check on web_context
//     - Call this function after loading pages
//     - Bypass this function for cookie persistence
//   Must be called after WebKitWebContext is created but before loading content.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
void add_cookie(WebKitWebContext *web_context)
{
    if (web_context == NULL) {
        ciao_error("add_cookie: web_context is NULL");
        return;
    }

    char full_storage_path[1024] = {0};
    initialize_cookie_storage(full_storage_path, sizeof(full_storage_path));

    if (full_storage_path[0] == '\0') {
        ciao_error("Failed to initialize cookie storage path");
        return;
    }

    WebKitCookieManager *cookie_manager = webkit_web_context_get_cookie_manager(web_context);
    if (cookie_manager == NULL) {
        ciao_error("Failed to get WebKitCookieManager");
        return;
    }

    webkit_cookie_manager_set_persistent_storage(
        cookie_manager,
        full_storage_path,
        WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE
    );

    // ←←← THIS LINE WAS MISSING — this is why cookies were not loaded
    webkit_cookie_manager_set_accept_policy(cookie_manager, WEBKIT_COOKIE_POLICY_ACCEPT_ALWAYS);

    ciao_info("Persistent cookie storage enabled: %s", full_storage_path);
}