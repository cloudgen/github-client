// =========================================================================
// CIAO DEFENSIVE CODING STYLE - URL BAR PAGE UPDATE HEADER
// =========================================================================

#ifndef URL_BAR_PAGE_H
#define URL_BAR_PAGE_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// Forward declaration via central header
#include "url_bar_callbacks.h"

// =========================================================================
// Flexibility Note:
//   Both on_uri_changed() and on_load_changed() are available to any file
//   that includes this header. This allows easy switching between
//   url_bar.c and url_bar_user_agent.c implementations.
// =========================================================================

#endif // URL_BAR_PAGE_H