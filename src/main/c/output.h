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
// General Purpose Requirement: Initialize Output System
// =========================================================================
void ciao_output_init(int argc, char **argv);

// =========================================================================
// Core logging function - Single Source of Output
// =========================================================================
void ciao_log(CiaoOutputLevel level, const char *format, ...);

// =========================================================================
// Convenience functions (Single Point of Entry)
// =========================================================================
void ciao_info(const char *format, ...);
void ciao_warn(const char *format, ...);
void ciao_error(const char *format, ...);
void ciao_debug(const char *format, ...);     // Controlled by DEBUG=1

void ciao_print_help(void);

#endif // OUTPUT_H