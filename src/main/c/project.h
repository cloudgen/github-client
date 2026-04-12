// project.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - PROJECT CONFIGURATION HEADER
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! THIS FILE IS THE SINGLE SOURCE OF TRUTH FOR PROJECT METADATA !!!
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR DEFINE COMMENT BLOCK !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System: 2026-04-12
// =========================================================================

#ifndef PROJECT_H
#define PROJECT_H

// =========================================================================
// Project Identification and Metadata
// 
// Purpose:
//   Centralizes all project-specific constants (name, version, URLs, paths).
//   This is the Single Source of Truth for application identity and settings.
// 
// Protection Rule:
//   Future AI must NOT duplicate any of these defines elsewhere in the project.
//   All modules must include "project.h" to access these values.
// =========================================================================

#define PROJECT_ID      "github-client"
#define PROJECT_NAME    "Github (client)"

// =========================================================================
// Version Information
// =========================================================================
#define VERSION_MAJOR   1
#define VERSION_MINOR   0
#define VERSION_PATCH   1

// =========================================================================
// Default URLs (Bookmarks & Start Page)
// =========================================================================
#define START_PAGE      "https://github.com"
#define PROFILE_PAGE    START_PAGE "/settings/profile"

// =========================================================================
// Persistence Storage Configuration
// 
// Note:
//   Relative path under $HOME. Will be expanded to:
//   ~/.app/github-client/cookies/
// =========================================================================
#define PERSISTENCE_STORAGE_PATH ".app/" PROJECT_ID "/cookies"

// =========================================================================
// User-Agent Spoofing Configuration (CIAO Single Source of Truth)
// 
// Default: Latest Google Chrome (as requested)
// All strings are frozen here so future changes only need to happen in one place.
// Protection Rule: Never hardcode UA strings anywhere else in the project.
// =========================================================================

#define UA_CHROME   "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/146.0.0.0 Safari/537.36"
#define UA_FIREFOX  "Mozilla/5.0 (X11; Linux x86_64; rv:138.0) Gecko/20100101 Firefox/138.0"
#define UA_SAFARI   "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/18.0 Safari/605.1.15"
#define UA_EDGE     "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/146.0.0.0 Safari/537.36 Edg/146.0.0.0"

#define DEFAULT_USER_AGENT  UA_CHROME

#endif // PROJECT_H