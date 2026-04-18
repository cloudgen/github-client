// =========================================================================
// CIAO DEFENSIVE CODING STYLE - URL BAR GO BUTTON HEADER
// =========================================================================

#ifndef URL_BAR_GO_BUTTON_H
#define URL_BAR_GO_BUTTON_H

#include <gtk/gtk.h>

// Forward declaration via central header (for maximum flexibility)
#include "url_bar_callbacks.h"

// =========================================================================
// Public API Note:
//   This callback is intentionally declared with external linkage so it can
//   be connected from url_bar_user_agent.c, url_bar.c, or any other file
//   without modification.
//
//   This supports incomplete integration scenarios across projects.
// =========================================================================

#endif // URL_BAR_GO_BUTTON_H