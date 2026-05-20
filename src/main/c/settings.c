// =========================================================================
// CIAO DEFENSIVE CODING STYLE - SETTINGS PERSISTENCE MODULE (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-18
// =========================================================================

#include "settings.h"
#include "output.h"
#include "project.h"
#include "config.h"
#include "path_utils.h"
#include <glib.h>
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>

// ──────────────────────────────────────────────
// debug_report_settings_structure() - Debug: Show settings.sqlite content
// ──────────────────────────────────────────────
//
// Purpose:
//   When DEBUG=1, prints the full structure (all key = value pairs) from settings.sqlite.
//   Called automatically on every program startup.
//
// CIAO-Lite:
//   - Caution: Safe SQLite handling
//   - Intentional: Clear key=value output for easy inspection
//
// Last updated: May 2026
// ──────────────────────────────────────────────
void debug_report_settings_structure(void)
{
    char db_path[1024] = {0};
    initialize_settings_storage(db_path, sizeof(db_path));

    sqlite3 *db = NULL;
    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        ciao_debug("[DEBUG] Could not open settings.sqlite");
        return;
    }

    sqlite3_stmt *stmt = NULL;
    const char *sql = "SELECT key, value FROM settings ORDER BY key;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        ciao_debug("[DEBUG] === settings.sqlite structure ===");
        int row = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *key   = (const char*)sqlite3_column_text(stmt, 0);
            const char *value = (const char*)sqlite3_column_text(stmt, 1);
            ciao_debug("[DEBUG]   %s = %s", key ? key : "(null)", value ? value : "(null)");
            row++;
        }
        if (row == 0) {
            ciao_debug("[DEBUG]   (settings.sqlite is empty - first run)");
        }
        sqlite3_finalize(stmt);
    } else {
        ciao_debug("[DEBUG] settings.sqlite: table not yet created");
    }
    sqlite3_close(db);
}

// ──────────────────────────────────────────────
// ciao_create_storage_directories() - Internal helper: Create storage directory
// ──────────────────────────────────────────────
//
// Purpose:
//   Creates the full directory tree for persistent settings storage safely.
//   This is an internal helper duplicated for module independence.
//
// Parameters:
//   Input:
//     - path : const char* - Full directory path to create (must not be NULL or empty)
//   Output/Return: None (void function, creates directories on disk)
//
// Dependencies:
//   - Functions called: ciao_error(), ciao_debug(), g_mkdir_with_parents()
//   - Headers required: glib.h, output.h, settings.h
//   - External: GNOME/GLib filesystem functions
//
// Note:
//   This is intentionally duplicated from cookies.c so the settings module
//   remains completely self-contained (CIAO Principle 6 - General Purpose).
//   Future AI must NOT remove this helper or replace it with manual mkdir.
//
// CIAO Principles Applied:
//   - Caution: NULL/empty path validation and proper error reporting
//   - Intentional: Keeps settings module independent
//   - Anti-fragile: Defends against path issues and permission problems
//   - Over-protect: Dedicated helper to isolate directory creation logic
//
// Protection Rule (Sacred):
//   Future AI must NOT remove this helper or replace it with manual strtok/mkdir loops.
//   This function defends against path traversal and permission issues.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
static void ciao_create_storage_directories(const char *path)
{
    if (path == NULL || *path == '\0') {
        ciao_error("Cannot create settings directories: path is empty");
        return;
    }

    gint result = g_mkdir_with_parents(path, 0700);
    if (result == -1) {
        ciao_error("Failed to create settings directory tree: %s", path);
    } else {
        ciao_debug("Settings storage directories ensured: %s", path);
    }
}

// ──────────────────────────────────────────────
// initialize_settings_storage() - Initialize settings storage path
// ──────────────────────────────────────────────
//
// Purpose:
//   Builds the full path to the settings.sqlite database and ensures
//   the directory structure exists. The settings.sqlite is stored in the
//   SAME directory as cookies.sqlite.
//
// Parameters:
//   Input:
//     - full_settings_db_path : char* - Output buffer to receive the full path
//     - max_len               : size_t - Size of the output buffer (recommended: 1024)
//   Output/Return:
//     - Fills full_settings_db_path with the constructed path (empty on failure)
//
// Dependencies:
//   - Functions called: ciao_error(), ciao_debug(), resolve_full_storage_path(),
//     ciao_create_storage_directories(), config_get_base_path(), g_snprintf()
//   - Headers required: glib.h, stdlib.h, settings.h, project.h, output.h, path_utils.h
//   - External: Relies on config_get_base_path() and the new safe path resolver
//
// Why This Design:
//   Now uses the centralized resolve_full_storage_path() to eliminate the
//   critical tilde expansion bug permanently. Settings module remains
//   self-contained while sharing the same storage location as cookies.
//
// CIAO Principles Applied:
//   - Caution: Buffer safety, full validation, no fragile manual tilde logic
//   - Intentional: Single Point of Entry for settings storage initialization
//   - Anti-fragile: Graceful degradation when HOME is not set
//   - Over-protect: Uses dedicated path_utils module to prevent future regressions
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Remove the call to resolve_full_storage_path()
//     - Reintroduce manual tilde expansion (g_str_has_prefix + base+2)
//     - Hardcode paths instead of using config + path_utils
//   This function must always go through the new path resolver.
//
// Last updated: 2026-05-17 (fixed tilde bug using path_utils)
// ──────────────────────────────────────────────
void initialize_settings_storage(char *full_settings_db_path, size_t max_len)
{
    if (full_settings_db_path == NULL || max_len == 0) {
        ciao_error("initialize_settings_storage: invalid buffer");
        return;
    }

    const char *base = config_get_base_path();
    if (base == NULL || *base == '\0') {
        base = PERSISTENCE_STORAGE_PATH;   // from project.h
    }

    // Use the new safe path resolver (Single Point of Truth for tilde expansion)
    resolve_full_storage_path(base, full_settings_db_path, max_len);

    if (full_settings_db_path[0] == '\0') {
        ciao_error("Failed to resolve settings storage path");
        return;
    }

    ciao_create_storage_directories(full_settings_db_path);

    // Append settings filename
    char final_path[1024] = {0};
    g_snprintf(final_path, sizeof(final_path), "%s/%s", full_settings_db_path, SETTINGS_DB_FILENAME);
    g_strlcpy(full_settings_db_path, final_path, max_len);

    ciao_debug("Settings DB path initialized: %s", full_settings_db_path);
}

// ──────────────────────────────────────────────
// open_settings_db() - Internal helper: Open (and initialise) the settings SQLite database
// ──────────────────────────────────────────────
//
// Purpose:
//   Opens (and creates if necessary) the settings.sqlite database and ensures
//   the required table exists. This is the Single Point of Truth for database access.
//
// Parameters:
//   Input: None
//   Output/Return:
//     - sqlite3* - Pointer to open database connection, or NULL on failure
//
// Dependencies:
//   - Functions called: initialize_settings_storage(), sqlite3_open(), sqlite3_exec(),
//     ciao_error(), sqlite3_errmsg(), sqlite3_close(), sqlite3_free()
//   - Headers required: sqlite3.h, settings.h, output.h, project.h
//
// Why This Design:
//   Centralizes database opening and schema initialization for all settings operations.
//
// Protection Rule (Sacred):
//   Future AI must NOT bypass this helper for database access.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
static sqlite3* open_settings_db(void)
{
    char db_path[1024] = {0};
    initialize_settings_storage(db_path, sizeof(db_path));

    if (db_path[0] == '\0') {
        return NULL;
    }

    sqlite3 *db = NULL;
    int rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        ciao_error("Failed to open settings DB: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    // Ensure table exists (Single Source of Truth for schema)
    const char *create_sql = "CREATE TABLE IF NOT EXISTS settings ("
                             "key TEXT PRIMARY KEY, "
                             "value TEXT NOT NULL);";

    char *err_msg = NULL;
    rc = sqlite3_exec(db, create_sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        ciao_error("Failed to create settings table: %s", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return NULL;
    }

    return db;
}

// ──────────────────────────────────────────────
// load_user_agent_from_settings() - Load the persisted User-Agent
// ──────────────────────────────────────────────
//
// Purpose:
//   Loads the persisted User-Agent string from settings.sqlite.
//   If no value exists, the buffer will be empty (caller should use DEFAULT_USER_AGENT).
//
// Parameters:
//   Input:
//     - ua_buffer : char* - Output buffer to receive the User-Agent string
//     - max_len   : size_t - Size of the output buffer (recommended: 1024)
//   Output/Return:
//     - Fills ua_buffer with the saved User-Agent or empty string
//
// Dependencies:
//   - Functions called: open_settings_db(), sqlite3_prepare_v2(), sqlite3_bind_text(),
//     sqlite3_step(), sqlite3_column_text(), g_strlcpy(), sqlite3_finalize(), sqlite3_close(),
//     ciao_error(), ciao_debug()
//   - Headers required: sqlite3.h, settings.h, output.h, project.h
//
// Why This Design:
//   Provides persistent User-Agent across application restarts.
//
// CIAO Principles Applied:
//   - Caution: Full buffer validation and error handling
//   - Intentional: Centralized loading of persisted settings
//   - Anti-fragile: Graceful fallback when no saved value exists
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
void load_user_agent_from_settings(char *ua_buffer, size_t max_len)
{
    if (ua_buffer == NULL || max_len == 0) {
        ciao_error("load_user_agent_from_settings: invalid buffer");
        return;
    }
    ua_buffer[0] = '\0';

    sqlite3 *db = open_settings_db();
    if (!db) return;

    const char *sql = "SELECT value FROM settings WHERE key = ?;";
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, SETTINGS_KEY_USER_AGENT, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *value = sqlite3_column_text(stmt, 0);
        if (value) {
            g_strlcpy(ua_buffer, (const char*)value, max_len);
            ciao_debug("Loaded User-Agent from settings DB");
        }
    } else {
        ciao_debug("No saved User-Agent found (will use DEFAULT_USER_AGENT)");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

// ──────────────────────────────────────────────
// save_user_agent_to_settings() - Save User-Agent to persistent storage
// ──────────────────────────────────────────────
//
// Purpose:
//   Saves the current User-Agent string to settings.sqlite using INSERT OR REPLACE.
//   Called automatically whenever the User-Agent is changed via the menu.
//
// Parameters:
//   Input:
//     - ua_string : const char* - The User-Agent string to persist (must not be empty)
//   Output/Return: None
//
// Dependencies:
//   - Functions called: open_settings_db(), sqlite3_prepare_v2(), sqlite3_bind_text(),
//     sqlite3_step(), sqlite3_finalize(), sqlite3_close(), ciao_error(), ciao_info()
//   - Headers required: sqlite3.h, settings.h, output.h, project.h
//
// Why This Design:
//   Ensures User-Agent preference survives application restarts.
//
// CIAO Principles Applied:
//   - Caution: Input validation and proper SQLite error handling
//   - Intentional: Persistent storage for User-Agent selection
//   - Anti-fragile: Safe no-op on database failures
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
void save_user_agent_to_settings(const char *ua_string)
{
    if (ua_string == NULL || *ua_string == '\0') {
        ciao_error("save_user_agent_to_settings: invalid UA string");
        return;
    }

    sqlite3 *db = open_settings_db();
    if (!db) return;

    const char *sql = "INSERT OR REPLACE INTO settings (key, value) VALUES (?, ?);";
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, SETTINGS_KEY_USER_AGENT, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, ua_string, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        ciao_error("Failed to save User-Agent to settings DB");
    } else {
        ciao_info("User-Agent persisted to settings.sqlite: %s", ua_string);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

