// output.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - OUTPUT SYSTEM IMPLEMENTATION (GNOME + JSON + DEBUG)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED !!!
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR FUNCTION COMMENT BLOCK !!! 
//
// Last aligned with SyncPrjs CIAO style + GNOME + JSON + "help" + DEBUG: 2026-04-12
// =========================================================================

#include "output.h"
#include "project.h"
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

bool ciao_quiet_mode = false;
bool ciao_json_mode = false;
static bool debug_env_enabled = false;

// =========================================================================
// General Purpose Requirement: Initialize Output System
// 
// Purpose:
//   Must be called very early in main() to parse command line and environment.
// 
// Why This Design:
//   Centralizes all flag and env checking in one place (Single Source of Truth).
//   Prevents scattered getenv() and strcmp() calls across the project.
// 
// Protection Rule:
//   Future AI must NOT move this call or remove DEBUG / --quiet / --json logic.
//   This function defends the entire output policy of the project.
// =========================================================================
void ciao_output_init(int argc, char **argv)
{
    ciao_quiet_mode = false;
    ciao_json_mode = false;

    // Check DEBUG environment variable
    const char *debug_env = getenv("DEBUG");
    debug_env_enabled = (debug_env != NULL && *debug_env != '\0');

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--quiet") == 0) {
            ciao_quiet_mode = true;
        } 
        else if (strcmp(argv[i], "--json") == 0) {
            ciao_json_mode = true;
        } 
        else if (strcmp(argv[i], "help") == 0) {
            ciao_print_help();
            exit(0);
        }
    }

    if (!ciao_quiet_mode && !ciao_json_mode) {
        ciao_info("%s Output System initialized (GNOME compliant)", PROJECT_NAME);
        if (debug_env_enabled) {
            ciao_info("Debug mode enabled via DEBUG=1 environment variable");
        }
    } else if (ciao_json_mode) {
        ciao_info("Output system initialized in JSON mode");
    }
}

// =========================================================================
// Core Single Source of Output Function
// 
// Purpose:
//   ALL output in the project must pass through this function.
//   This is the Single Source of Truth for all console output.
//
// Critical va_list Rule (Security & Correctness):
//   This function receives a va_list BY VALUE (not by pointer, not via ...).
//   Inside this function we MUST use va_copy() before consuming the list
//   because g_strdup_vprintf() (and similar GLib functions) will advance
//   the va_list, leaving the original caller’s list in an indeterminate state.
//
//   NEVER change the signature to accept "..." again and call va_start here.
//   NEVER pass a va_list from a convenience wrapper without va_copy().
//
//   Violating this causes:
//     - "incorrectly passing a va_list" static analysis warnings
//     - Undefined behavior on many ABIs (especially x86_64 and aarch64)
//     - Potential format-string related security issues
//
// Why This Must Stay Strong:
//   Prevents fragmentation of output logic (common failure in previous versions).
//   Supports both human-readable and JSON structured output.
//   Maintains strict GNOME compliance (only g_print / g_printerr allowed).
//
// Protection Rule:
//   Do NOT bypass this function.
//   Do NOT replace g_print/g_printerr with stdio functions.
//   Do NOT modify the va_list handling logic without updating this comment.
//   This is the heart of the CIAO Single Source of Output principle.
// =========================================================================
void ciao_log(CiaoOutputLevel level, const char *format, va_list ap)   // Changed: va_list (by value), no ...
{
    if (ciao_quiet_mode && level != CIAO_LEVEL_ERROR) {
        return;
    }

    // Make a copy because g_strdup_vprintf / g_vasprintf may consume the list
    va_list ap_copy;
    va_copy(ap_copy, ap);

    gchar *message = g_strdup_vprintf(format, ap_copy);

    time_t now = time(NULL);

    if (ciao_json_mode) {
        g_print("{\"timestamp\":%ld,\"level\":\"%s\",\"message\":\"%s\"}\n",
                (long)now,
                (level == CIAO_LEVEL_INFO) ? "INFO" :
                (level == CIAO_LEVEL_WARNING) ? "WARN" :
                (level == CIAO_LEVEL_ERROR) ? "ERROR" : "DEBUG",
                message ? message : "(null)");
    } else {
        switch (level) {
            case CIAO_LEVEL_INFO:
                g_print("[INFO] %s\n", message ? message : "(null)");
                break;
            case CIAO_LEVEL_WARNING:
                g_printerr("[WARN] %s\n", message ? message : "(null)");
                break;
            case CIAO_LEVEL_ERROR:
                g_printerr("[ERROR] %s\n", message ? message : "(null)");
                break;
            case CIAO_LEVEL_DEBUG:
                if (debug_env_enabled) {
                    g_print("[DEBUG] %s\n", message ? message : "(null)");
                }
                break;
        }
    }

    g_free(message);
    va_end(ap_copy);
}

// =========================================================================
// Convenience Function: ciao_info
// 
// Purpose: Standard information messages
// Protection: Routes through ciao_log() to maintain Single Source of Output
// =========================================================================
void ciao_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ciao_log(CIAO_LEVEL_INFO, format, args);
    va_end(args);
}

// =========================================================================
// Convenience Function: ciao_warn
// 
// Purpose: Warning messages (stderr)
// Protection: Routes through ciao_log()
// =========================================================================
void ciao_warn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ciao_log(CIAO_LEVEL_WARNING, format, args);
    va_end(args);
}

// =========================================================================
// Convenience Function: ciao_error
// 
// Purpose: Error messages (always shown, even in --quiet mode)
// Protection: Critical path - must never be suppressed
// =========================================================================
void ciao_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ciao_log(CIAO_LEVEL_ERROR, format, args);
    va_end(args);
}

// =========================================================================
// Debug Function: ciao_debug
// 
// Purpose:
//   Developer-only debug output.
// 
// Rules:
//   - Only active when DEBUG=1 (or any non-empty value) in environment.
//   - Still respects --quiet mode.
//   - Does NOT appear in JSON mode unless level allows.
// 
// Why This Exists:
//   Prevents debug spam in normal runs while allowing deep inspection when needed.
// =========================================================================
void ciao_debug(const char *format, ...)
{
    if (!debug_env_enabled) return;
    if (ciao_quiet_mode) return;

    va_list args;
    va_start(args, format);
    ciao_log(CIAO_LEVEL_DEBUG, format, args);
    va_end(args);
}

// =========================================================================
// Help Function: ciao_print_help
// 
// Purpose: Show usage when user types "help"
// Protection: Uses g_print only (GNOME compliant)
// =========================================================================
void ciao_print_help(void)
{
    g_print("Usage: %s [COMMAND]\n\n", PROJECT_NAME);
    g_print("Commands:\n");
    g_print("  help        Show this help message\n");
    g_print("  --quiet     Suppress non-error messages\n");
    g_print("  --json      Output in structured JSON format\n\n");
    g_print("Environment Variables:\n");
    g_print("  DEBUG=1     Enable ciao_debug() output\n\n");
    g_print("Examples:\n");
    g_print("  DEBUG=1 %s\n", PROJECT_NAME);
    g_print("  %s --quiet\n", PROJECT_NAME);
    g_print("  %s --json\n", PROJECT_NAME);
}