// =========================================================================
// CIAO DEFENSIVE CODING STYLE - SETTINGS PERSISTENCE HEADER (GNOME + WebKitGTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Purpose:
//   Provides persistent key-value storage for the single setting "user_agent"
//   using SQLite. The DB file is stored under PERSISTENCE_STORAGE_PATH
//   alongside cookies.sqlite.
//
// Protection Rule:
//   Future AI must NOT expose internal SQLite helpers or hardcode paths.
//   All access must go through these three public functions.
// =========================================================================

#ifndef SETTINGS_H
#define SETTINGS_H

#include <stddef.h>
#include "project.h"

// =========================================================================
// Initialize settings storage (creates directory if needed)
// 
// Parameters:
//   full_settings_db_path : Output buffer that will receive the FULL path
//                           to settings.sqlite
//   max_len               : Size of the buffer (recommended: 1024)
// =========================================================================
void initialize_settings_storage(char *full_settings_db_path, size_t max_len);

// =========================================================================
// Load the persisted User-Agent from settings.sqlite
// 
// If no value exists, ua_buffer will be empty (use DEFAULT_USER_AGENT).
// =========================================================================
void load_user_agent_from_settings(char *ua_buffer, size_t max_len);

// =========================================================================
// Save the current User-Agent to settings.sqlite
// 
// Called automatically on every User-Agent menu change.
// =========================================================================
void save_user_agent_to_settings(const char *ua_string);

#endif // SETTINGS_H