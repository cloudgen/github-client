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
#include "config.h"
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

bool ciao_quiet_mode = false;
bool ciao_json_mode = false;
static bool debug_env_enabled = false;

// =========================================================================
// Global for auto-shutdown (Single Source of Truth)
// =========================================================================
int ciao_shutdown_seconds = 0;
int ciao_bookmark_index = 0;        // New: default = first bookmark (0)

// ──────────────────────────────────────────────
// ciao_output_init() - General Purpose Requirement: Initialize Output System
// ──────────────────────────────────────────────
//
// Purpose:
//   Must be called very early in main() to parse command line and environment.
//   This is the Single Point of Truth for application output configuration.
//
// Parameters:
//   Input:
//     - argc : int - Number of command line arguments
//     - argv : char** - Command line argument array
//   Output/Return: None (configures global output flags)
//
// Dependencies:
//   - Functions called: getenv(), atoi(), strncmp(), strcmp(), ciao_info(), ciao_debug(), ciao_print_help()
//   - Headers required: output.h, project.h, stdlib.h, string.h
//   - External: Parses --quiet, --json, --shutdown, --bookmark and environment variables
//
// New Features (2026-04-30):
//   - JSON=1 / QUIET=1 environment variables
//   - --bookmark=N / BOOKMARK=N  (0 = Start Page, 1 = Contacts, etc.)
//
// New Feature (2026-04-30):
//   --shutdown=N  or  SHUTDOWN=N  → auto exit after N seconds
//   Value 0 (or omitted) explicitly means "never shutdown".
//
// CIAO Principles Applied:
//   - Caution: Careful parsing with fallbacks and bounds checking
//   - Intentional: Early initialization and clear feature documentation
//   - Anti-fragile: Graceful handling of missing/invalid flags
//   - Over-protect: All output configuration centralized here
//
// Protection Rule (Sacred):
//   Future AI must NOT remove any existing flag/env logic.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
void ciao_output_init(int argc, char **argv)
{
    ciao_quiet_mode = false;
    ciao_json_mode = false;
    ciao_shutdown_seconds = 0;
    ciao_bookmark_index = 0;

    // Check DEBUG environment first
    const char *debug_env = getenv("DEBUG");
    debug_env_enabled = (debug_env != NULL && *debug_env != '\0');

    // Parse command line (highest priority)
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--quiet") == 0) {
            ciao_quiet_mode = true;
        } 
        else if (strcmp(argv[i], "--json") == 0) {
            ciao_json_mode = true;
        } 
        else if (strncmp(argv[i], "--shutdown=", 11) == 0) {
            int val = atoi(argv[i] + 11);
            if (val > 0) ciao_shutdown_seconds = val;
        }
        else if (strncmp(argv[i], "--bookmark=", 11) == 0) {
            ciao_bookmark_index = atoi(argv[i] + 11);
            if (ciao_bookmark_index < 0) ciao_bookmark_index = 0;
        }
        else if (strcmp(argv[i], "help") == 0) {
            ciao_print_help();
            exit(0);
        }
    }

    // Environment variables
    if (!ciao_quiet_mode) {
        const char *q = getenv("QUIET");
        if (q && (atoi(q) == 1 || strcmp(q, "1") == 0))
            ciao_quiet_mode = true;
    }
    if (!ciao_json_mode) {
        const char *j = getenv("JSON");
        if (j && (atoi(j) == 1 || strcmp(j, "1") == 0))
            ciao_json_mode = true;
    }

    // Respect config.conf [output] section
    if (!ciao_quiet_mode) ciao_quiet_mode = config_get_output_quiet();
    if (!ciao_json_mode)  ciao_json_mode  = config_get_output_json();
    if (!debug_env_enabled) debug_env_enabled = config_get_output_debug();

    // === NEW ENFORCEMENT RULE ===
    // When JSON mode is active, force quiet = true to prevent text pollution
    if (ciao_json_mode) {
        ciao_quiet_mode = true;
        ciao_debug("JSON mode active → quiet mode forced ON");
    }

    if (ciao_shutdown_seconds == 0) {
        const char *s = getenv("SHUTDOWN");
        if (s) ciao_shutdown_seconds = atoi(s);
        if (ciao_shutdown_seconds < 0) ciao_shutdown_seconds = 0;
    }

    if (ciao_bookmark_index == 0) {
        const char *b = getenv("BOOKMARK");
        if (b) {
            ciao_bookmark_index = atoi(b);
            if (ciao_bookmark_index < 0) ciao_bookmark_index = 0;
        }
    }

    if (ciao_shutdown_seconds > 0)
        ciao_info("Auto-shutdown enabled: will exit after %d seconds", ciao_shutdown_seconds);

    if (ciao_bookmark_index > 0)
        ciao_info("Will open bookmark index %d on startup", ciao_bookmark_index);

    if (!ciao_quiet_mode && !ciao_json_mode) {
        ciao_info("%s Output System initialized (GNOME compliant)", PROJECT_NAME);
        if (debug_env_enabled) {
            ciao_info("Debug mode enabled via DEBUG=1 environment variable");
        }
    } else if (ciao_json_mode) {
        ciao_info("Output system initialized in JSON mode");
    }
}

// ──────────────────────────────────────────────
// ciao_log() - Core Single Source of Output Function
// ──────────────────────────────────────────────
//
// Purpose:
//   ALL output in the project must pass through this function.
//   This is the Single Source of Truth for all console output.
//
// Parameters:
//   Input:
//     - level  : CiaoOutputLevel - Message severity
//     - format : const char* - printf-style format string
//     - ap     : va_list (by value) - Variable arguments
//   Output/Return: None (prints to stdout/stderr or JSON)
//
// Dependencies:
//   - Functions called: g_strdup_vprintf(), g_print(), g_printerr(), time(), va_copy(), va_end()
//   - Headers required: glib.h, stdarg.h, time.h, output.h
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
// Protection Rule (Sacred):
//   Do NOT bypass this function.
//   Do NOT replace g_print/g_printerr with stdio functions.
//   Do NOT modify the va_list handling logic without updating this comment.
//   This is the heart of the CIAO Single Source of Output principle.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
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

// ──────────────────────────────────────────────
// ciao_info() - Convenience Function: ciao_info
// ──────────────────────────────────────────────
//
// Purpose:
//   Standard information messages
//
// Parameters:
//   Input:
//     - format : const char* - printf-style format string
//     - ...    : Variable arguments
//   Output/Return: None
//
// Dependencies:
//   - Functions called: ciao_log()
//   - Headers required: output.h
//
// Protection:
//   Routes through ciao_log() to maintain Single Source of Output
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
void ciao_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ciao_log(CIAO_LEVEL_INFO, format, args);
    va_end(args);
}

// ──────────────────────────────────────────────
// ciao_warn() - Convenience Function: ciao_warn
// ──────────────────────────────────────────────
//
// Purpose:
//   Warning messages (stderr)
//
// Parameters:
//   Input:
//     - format : const char* - printf-style format string
//     - ...    : Variable arguments
//   Output/Return: None
//
// Dependencies:
//   - Functions called: ciao_log()
//   - Headers required: output.h
//
// Protection:
//   Routes through ciao_log()
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
void ciao_warn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ciao_log(CIAO_LEVEL_WARNING, format, args);
    va_end(args);
}

// ──────────────────────────────────────────────
// ciao_error() - Convenience Function: ciao_error
// ──────────────────────────────────────────────
//
// Purpose:
//   Error messages (always shown, even in --quiet mode)
//
// Parameters:
//   Input:
//     - format : const char* - printf-style format string
//     - ...    : Variable arguments
//   Output/Return: None
//
// Dependencies:
//   - Functions called: ciao_log()
//   - Headers required: output.h
//
// Protection:
//   Critical path - must never be suppressed
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
void ciao_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    ciao_log(CIAO_LEVEL_ERROR, format, args);
    va_end(args);
}

// ──────────────────────────────────────────────
// ciao_debug() - Debug Function: ciao_debug
// ──────────────────────────────────────────────
//
// Purpose:
//   Developer-only debug output.
//
// Parameters:
//   Input:
//     - format : const char* - printf-style format string
//     - ...    : Variable arguments
//   Output/Return: None
//
// Dependencies:
//   - Functions called: ciao_log()
//   - Headers required: output.h
//
// Rules:
//   - Only active when DEBUG=1 (or any non-empty value) in environment.
//   - Still respects --quiet mode.
//   - Does NOT appear in JSON mode unless level allows.
//
// Why This Exists:
//   Prevents debug spam in normal runs while allowing deep inspection when needed.
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
void ciao_debug(const char *format, ...)
{
    if (!debug_env_enabled) return;
    if (ciao_quiet_mode) return;

    va_list args;
    va_start(args, format);
    ciao_log(CIAO_LEVEL_DEBUG, format, args);
    va_end(args);
}

// ──────────────────────────────────────────────
// ciao_print_help() - Help Function: ciao_print_help
// ──────────────────────────────────────────────
//
// Purpose:
//   Show usage when user types "help"
//
// Parameters:
//   Input: None
//   Output/Return: None
//
// Dependencies:
//   - Functions called: g_print()
//   - Headers required: glib.h
//
// Protection:
//   Uses g_print only (GNOME compliant)
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
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

