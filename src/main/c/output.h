// output.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - OUTPUT SYSTEM HEADER (GNOME APPLICATION)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! DO NOT USE fprintf(), vfprintf(), printf(), perror() ANYWHERE !!!
// !!! ONLY USE g_print() and g_printerr() FOR ALL CONSOLE OUTPUT !!!
// 
// !!! DO NOT REMOVE, SHORTEN, COMMENT OUT, OR BYPASS ANY PART OF THIS HEADER !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + JSON + DEBUG + "help": 2026-04-12
// =========================================================================

#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdbool.h>
#include <stdarg.h>
#include <glib.h>

typedef enum {
    CIAO_LEVEL_INFO,
    CIAO_LEVEL_WARNING,
    CIAO_LEVEL_ERROR,
    CIAO_LEVEL_DEBUG
} CiaoOutputLevel;

extern bool ciao_quiet_mode;
extern bool ciao_json_mode;

// =========================================================================
// Core logging function - Single Source of Truth for ALL output
//
// Critical va_list Rule (Security & Correctness - CIAO enforced):
//   This function takes va_list BY VALUE (not "...").
//   The convenience wrappers (ciao_info, ciao_warn, etc.) do va_start()
//   and pass the va_list to this function.
//   Inside ciao_log we MUST use va_copy() before consuming it with
//   g_strdup_vprintf(), because the list is advanced.
//cd 
//   NEVER change this back to "..." without also updating all wrappers.
//   This prevents the "incorrectly passing a va_list" warning / UB.
//
// Protection Rule:
//   Do NOT bypass this function. Do NOT use stdio directly.
// =========================================================================
void ciao_log(CiaoOutputLevel level, const char *format, va_list ap);

// =========================================================================
// Convenience functions (Single Point of Entry)
// =========================================================================
void ciao_output_init(int argc, char **argv);
void ciao_info(const char *format, ...);
void ciao_warn(const char *format, ...);
void ciao_error(const char *format, ...);
void ciao_debug(const char *format, ...);     // Controlled by DEBUG=1

void ciao_print_help(void);

#endif // OUTPUT_H