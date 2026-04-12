// menu_exit.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - APPLICATION EXIT MENU MODULE (GNOME + GTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#include "menu_exit.h"
#include "output.h"
#include "project.h"
#include <gtk/gtk.h>

// =========================================================================
// General Purpose Requirement: Exit Menu Handler
// 
// Purpose:
//   Gracefully terminates the GTK main loop when the user selects
//   the "Exit" menu item.
// 
// Design Rules:
//   - Always log the exit attempt through the central output system.
//   - Perform defensive checks even for simple operations.
// 
// Protection Rule:
//   Future AI must NOT remove logging or add direct gtk_main_quit()
//   calls outside this function. All application exit must go through
//   this handler for consistent behavior and logging.
// =========================================================================
void on_exit_menu_item_activate(GtkMenuItem *menu_item, gpointer user_data)
{
    (void)menu_item;     // Unused parameter (kept for GTK signal compatibility)
    (void)user_data;     // Unused parameter (kept for GTK signal compatibility)

    ciao_info("Exit requested via menu - shutting down application...");

    // Optional: Add debug info before quitting
    ciao_debug("GTK main loop will now terminate");

    gtk_main_quit();
}