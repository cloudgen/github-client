// dialog_version.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - VERSION DIALOG MODULE (GNOME + GTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// !!! GUI DIALOGS ARE ALLOWED BUT MUST BE DEFENSIVE AND WELL DOCUMENTED !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#include "dialog_version.h"
#include "output.h"
#include "project.h"
#include <gtk/gtk.h>

// =========================================================================
// General Purpose Requirement: Show Version Dialog
// 
// Purpose:
//   Displays an information dialog with the current application version
//   when the user activates the "About / Version" menu item.
// 
// Design Rules:
//   - Always use a transient parent (the main window) for proper modality.
//   - Use GTK_MESSAGE_INFO and GTK_BUTTONS_OK as per GNOME HIG.
//   - All errors or unexpected states are reported through ciao_error().
// 
// Protection Rule:
//   Future AI must NOT remove NULL checks, transient parent, or destroy logic.
//   This function defends against crashes when called with invalid parent.
// =========================================================================
void on_show_version_activate(GtkMenuItem *menu_item, gpointer user_data)
{
    // Defensive check: Ensure we have a valid parent window
    if (user_data == NULL) {
        ciao_error("on_show_version_activate: user_data (parent window) is NULL");
        return;
    }

    GtkWindow *parent = GTK_WINDOW(user_data);

    // Create the version dialog
    GtkWidget *dialog = gtk_message_dialog_new(
        parent,
        GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Version: %d.%d.%d",
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_PATCH
    );

    if (dialog == NULL) {
        ciao_error("Failed to create version dialog");
        return;
    }

    // Set additional dialog properties for better UX
    gtk_window_set_title(GTK_WINDOW(dialog), "About " PROJECT_NAME);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

    ciao_debug("Version dialog displayed: %d.%d.%d", 
               VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    // Run the dialog and wait for user response
    gtk_dialog_run(GTK_DIALOG(dialog));

    // Clean up
    gtk_widget_destroy(dialog);
}