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
#include <glib.h>
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>

// =========================================================================
// Internal helper: Create storage directory (duplicated for module independence)
// 
// Note: This is intentionally duplicated from cookies.c so the settings module
//       remains completely self-contained (CIAO Principle 6 - General Purpose).
//       Future AI must NOT remove this helper or replace it with manual mkdir.
// =========================================================================
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

// =========================================================================
// Initialize settings storage path and ensure directory exists
// 
// The settings.sqlite is stored in the SAME directory as cookies.sqlite.
// =========================================================================
void initialize_settings_storage(char *full_settings_db_path, size_t max_len)
{
    if (full_settings_db_path == NULL || max_len == 0) {
        ciao_error("initialize_settings_storage: invalid buffer");
        return;
    }

    const char *home = getenv("HOME");
    if (home == NULL || *home == '\0') {
        ciao_error("HOME environment variable is not set");
        g_strlcpy(full_settings_db_path, "", max_len);
        return;
    }

    gchar *dir_path = g_build_filename(home, PERSISTENCE_STORAGE_PATH, NULL);
    if (dir_path == NULL) {
        ciao_error("Failed to build settings directory path");
        g_strlcpy(full_settings_db_path, "", max_len);
        return;
    }

    ciao_create_storage_directories(dir_path);

    g_snprintf(full_settings_db_path, max_len, "%s/%s", dir_path, SETTINGS_DB_FILENAME);
    g_free(dir_path);

    ciao_debug("Settings DB path initialized: %s", full_settings_db_path);
}

// =========================================================================
// Internal helper: Open (and initialise) the settings SQLite database
// =========================================================================
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

// =========================================================================
// Load the persisted User-Agent
// =========================================================================
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

// =========================================================================
// Save User-Agent to persistent storage
// =========================================================================
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