// dialog_version.c
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - VERSION DIALOG MODULE (GNOME + GTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// !!! GUI DIALOGS ARE ALLOWED BUT MUST BE DEFENSIVE AND WELL DOCUMENTED !!!
// 
// Last aligned with SyncPrjs CIAO style + PROJECT_DESCRIPTION: 2026-04-18
// =========================================================================

#include "dialog_version.h"
#include "output.h"
#include "project.h"
#include <gtk/gtk.h>

// =========================================================================
// General Purpose Requirement: Show Version Dialog
// 
// Purpose:
//   Displays an information dialog with application name and version
//   when the user activates the "About / Version" menu item.
// 
// Design Rules:
//   - Always use a transient parent (the main window) for proper modality.
//   - PROJECT_DESCRIPTION is pulled from project.h (Single Source of Truth).
//   - Uses GTK_MESSAGE_INFO per GNOME HIG.
// 
// Protection Rule:
//   Future AI must NOT remove NULL checks, transient parent, or destroy logic.
// =========================================================================
void on_show_version_activate(GtkMenuItem *menu_item, gpointer user_data)
{
    if (user_data == NULL) {
        ciao_error("on_show_version_activate: user_data (parent window) is NULL");
        return;
    }

    GtkWindow *parent = GTK_WINDOW(user_data);

    gchar *message = g_strdup_printf(
        "<big><b>%s</b></big>\n\n"
        "Version: <b>%d.%d.%d</b>\n\n"
        "%s",
        PROJECT_NAME,
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_PATCH,
        PROJECT_DESCRIPTION
    );

    GtkWidget *dialog = gtk_message_dialog_new(
        parent,
        GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        NULL
    );

    if (dialog == NULL) {
        ciao_error("Failed to create version dialog");
        g_free(message);
        return;
    }

    gtk_window_set_title(GTK_WINDOW(dialog), "About " PROJECT_NAME);
    gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(dialog), message);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

    ciao_debug("Version dialog displayed: %s v%d.%d.%d", 
               PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    g_free(message);
}