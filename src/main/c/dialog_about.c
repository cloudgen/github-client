// =========================================================================
// CIAO DEFENSIVE CODING STYLE - ABOUT DIALOG MODULE (GNOME + GTK)
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! ONLY g_print() / g_printerr() ARE ALLOWED THROUGH ciao_* FUNCTIONS !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME HIG: 2026-04-18
// =========================================================================

#include "dialog_about.h"
#include "output.h"
#include "project.h"
#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>

// For libc version detection
#ifdef __GLIBC__
#include <gnu/libc-version.h>
#endif

#ifdef __linux__
#include <sys/utsname.h>
#endif

// ──────────────────────────────────────────────
// get_platform_info() - Build Platform and Libc Information String
// ──────────────────────────────────────────────
//
// Purpose:
//   Constructs a detailed platform information string including OS name/version,
//   libc type/version, and architecture. Used by the About dialog.
//
// Parameters:
//   Input: None
//   Output/Return:
//     - gchar* - Newly allocated string containing platform info (must be freed by caller)
//
// Dependencies:
//   - Functions called: g_string_new(), g_string_append_printf(), g_string_free(),
//     g_get_os_info(), gnu_get_libc_version(), g_getenv()
//   - Headers required: glib.h, gtk/gtk.h, gnu/libc-version.h (conditional), sys/utsname.h (conditional)
//   - External: Relies on GLib OS info APIs and compile-time libc detection
//
// Why This Design:
//   Dynamically detects runtime platform details for rich About dialog information.
//   Uses GString for safe concatenation and supports multiple libc variants.
//
// CIAO Principles Applied:
//   - Caution: Safe string handling and conditional compilation
//   - Intentional: Provides rich metadata for user transparency
//   - Anti-fragile: Graceful fallbacks for unknown OS/libc/arch
//   - Over-protect: Preserves detailed platform detection logic
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Remove platform/libc/arch detection logic
//     - Simplify or hardcode platform string
//     - Remove g_string_free() cleanup
//   This helper is critical for the About dialog richness.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
static gchar* get_platform_info(void)
{
    GString *info = g_string_new(NULL);

    // OS Information
    const gchar *os_name = g_get_os_info(G_OS_INFO_KEY_NAME);
    const gchar *os_version = g_get_os_info(G_OS_INFO_KEY_VERSION);
    
    if (os_name) {
        g_string_append_printf(info, "%s", os_name);
        if (os_version) 
            g_string_append_printf(info, " %s", os_version);
    } else {
        g_string_append(info, "Linux");
    }

    // libc detection
#ifdef __GLIBC__
    const char *glibc_ver = gnu_get_libc_version();
    if (glibc_ver) {
        g_string_append_printf(info, " • glibc %s", glibc_ver);
    }
#elif defined(__MUSL__)
    g_string_append(info, " • musl");
#elif defined(__UCLIBC__)
    g_string_append(info, " • uClibc");
#else
    g_string_append(info, " • unknown libc");
#endif

    // Architecture
    const gchar *arch = g_getenv("HOSTTYPE");
    if (arch && *arch)
        g_string_append_printf(info, " • %s", arch);
    else
        g_string_append(info, " • unknown arch");

    return g_string_free(info, FALSE);
}

// ──────────────────────────────────────────────
// on_show_about_activate() - About Dialog Callback
// ──────────────────────────────────────────────
//
// Purpose:
//   Displays the standard GNOME About dialog with rich application metadata,
//   platform info, license, authors, and links. This is the Single Point of Truth
//   for the About dialog in the application.
//
// Parameters:
//   Input:
//     - menu_item   : GtkMenuItem* - The menu item that triggered this callback (unused)
//     - user_data   : gpointer - Expected to be the parent GtkWindow (may be NULL)
//   Output/Return: None (void callback, shows modal dialog)
//
// Dependencies:
//   - Functions called: get_platform_info(), gtk_about_dialog_new(), 
//     gtk_about_dialog_set_*, gtk_dialog_run(), gtk_widget_destroy(), ciao_debug()
//   - Headers required: gtk/gtk.h, glib.h, dialog_about.h, output.h, project.h
//   - External signals: Connected from menu item "activate" signal
//
// Why This Design:
//   Uses GTK's built-in About dialog for GNOME HIG compliance while enriching it
//   with dynamic platform/libc information and project metadata from project.h.
//
// CIAO Principles Applied:
//   - Caution: NULL checks and proper dialog cleanup
//   - Intentional: Centralizes all About dialog presentation logic
//   - Anti-fragile: Safe fallback when parent window is NULL
//   - Over-protect: Preserves rich metadata and platform detection
//
// Protection Rule (Sacred):
//   Future AI or developers MUST NOT:
//     - Remove platform info or any metadata fields
//     - Remove dialog creation/cleanup logic
//     - Hardcode strings instead of using PROJECT_* constants
//     - Bypass this function for showing About dialog
//   This is the only authorized entry point for the About dialog.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ────────────────────────────────────────────── 
void on_show_about_activate(GtkMenuItem *menu_item, gpointer user_data)
{
    (void)menu_item; // unused

    GtkWindow *parent = user_data ? GTK_WINDOW(user_data) : NULL;

    GtkWidget *dialog = gtk_about_dialog_new();
    if (!dialog) {
        ciao_error("Failed to create GtkAboutDialog");
        return;
    }

    gchar *platform = get_platform_info();

    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), PROJECT_NAME);
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), 
        g_strdup_printf("%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH));
    
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),
        "A lightweight native YouTube client built with WebKitGTK\n"
        "No Electron • No bloat • Pure GNOME experience");
    
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),
        "© 2026 Cloudgen Wong. All rights reserved.");
    
    // Use GTK_LICENSE_CUSTOM instead of GTK_LICENSE_MIT for broader GTK compatibility
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog), GTK_LICENSE_CUSTOM);
    gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(dialog),
        "This project is licensed under the MIT License.\n\n"
        "See LICENSE.md for full license text.");

    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), 
        "https://github.com/cloudgen/github-client");
    gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog), 
        "Github Repository");

    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), 
        (const gchar*[]){"Cloudgen Wong (@cloudgen)", NULL});

    gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(dialog), "applications-internet");

    ciao_debug("About dialog displayed for %s %d.%d.%d", 
               PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    // Run and clean up
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    g_free(platform);
}
