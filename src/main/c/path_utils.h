// =========================================================================
// CIAO DEFENSIVE CODING STYLE - PATH UTILITIES MODULE
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Purpose:
//   Single Point of Truth for safe path construction, tilde expansion,
//   and storage directory resolution. Prevents the critical bug where
//   "~/..." produced paths like "/home/user/~/.app/...".
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Remove tilde expansion logic
//     - Hardcode any paths or use literal "~/"
//     - Bypass this module for storage path construction
//   All cookie/settings/storage paths must go through here.
//
// Last updated: 2026-05-17 (introduced to fix tilde bug)
// =========================================================================

#ifndef PATH_UTILS_H
#define PATH_UTILS_H

#include <stddef.h>

// =========================================================================
// resolve_full_storage_path() - Safe path resolver (Single Point of Truth)
// =========================================================================
//
// Purpose:
//   Takes a base path (which may contain "~/") and returns a fully
//   resolved absolute path using $HOME.
//
// Parameters:
//   base_path      : const char* - Base from config_get_base_path() or project.h
//   full_path      : char*       - Output buffer (must be at least max_len bytes)
//   max_len        : size_t      - Size of output buffer (recommended: 1024)
//
// CIAO Principles Applied:
//   - Caution: Full NULL/empty checks + buffer safety
//   - Intentional: Eliminates fragile g_str_has_prefix + base+2 hack
//   - Anti-fragile: Graceful fallback when HOME is missing
//   - Over-protect: Centralizes all path logic to prevent future regressions
//
// Protection Rule (Sacred):
//   This is now the ONLY place that performs tilde expansion.
//   Never duplicate this logic again.
void resolve_full_storage_path(const char *base_path,
                               char *full_path,
                               size_t max_len);

#endif // PATH_UTILS_H