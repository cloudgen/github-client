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

// ──────────────────────────────────────────────
// on_exit_menu_item_activate() - Exit Menu Handler
// ──────────────────────────────────────────────
//
// Purpose:
//   Gracefully terminates the GTK main loop when the user selects
//   the "Exit" menu item.
//
// Parameters:
//   Input:
//     - menu_item : GtkMenuItem* - The menu item that triggered this callback (unused)
//     - user_data : gpointer - User data passed during signal connection (unused)
//   Output/Return: None (void callback)
//
// Dependencies:
//   - Functions called: ciao_info(), ciao_debug(), gtk_main_quit()
//   - Headers required: gtk/gtk.h, output.h, menu_exit.h
//   - External: Connected from "Exit" menu item "activate" signal
//
// Design Rules:
//   - Always log the exit attempt through the central output system.
//   - Perform defensive checks even for simple operations.
//
// CIAO Principles Applied:
//   - Caution: Defensive (void) casting and logging before exit
//   - Intentional: Centralized exit point for consistent behavior
//   - Anti-fragile: Safe no-op pattern with proper GTK signal compatibility
//   - Over-protect: Forces all exits to go through this handler
//
// Protection Rule (Sacred):
//   Future AI must NOT remove logging or add direct gtk_main_quit()
//   calls outside this function. All application exit must go through
//   this handler for consistent behavior and logging.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
void on_exit_menu_item_activate(GtkMenuItem *menu_item, gpointer user_data)
{
    (void)menu_item;     // Unused parameter (kept for GTK signal compatibility)
    (void)user_data;     // Unused parameter (kept for GTK signal compatibility)

    ciao_info("Exit requested via menu - shutting down application...");

    // Optional: Add debug info before quitting
    ciao_debug("GTK main loop will now terminate");

    gtk_main_quit();
}