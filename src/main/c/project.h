// project.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - PROJECT CONFIGURATION HEADER
// =========================================================================
//
// !!! THIS IS A GNOME/GTK APPLICATION !!!
// !!! THIS FILE IS THE SINGLE SOURCE OF TRUTH FOR PROJECT METADATA !!!
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR DEFINE COMMENT BLOCK !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + Output System + Settings: 2026-04-18
// =========================================================================

#ifndef PROJECT_H
#define PROJECT_H

// =========================================================================
// Project Identification and Metadata
// =========================================================================
#define PROJECT_ID      "github-client"
#define PROJECT_NAME    "Github (client)"

// =========================================================================
// Version Information
// =========================================================================
#define VERSION_MAJOR   1
#define VERSION_MINOR   0
#define VERSION_PATCH   7

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
//   ~/.app/gmail-client/cookies/
// =========================================================================
#define PERSISTENCE_STORAGE_PATH ".app/" PROJECT_ID "/cookies"

// =========================================================================
// Project Description (Single Source of Truth for UI strings)
// =========================================================================
#define PROJECT_DESCRIPTION "A lightweight native Github client built with WebKitGTK."

// =========================================================================
// User-Agent Spoofing Configuration (CIAO Single Source of Truth)
// 
// Default: Latest Google Chrome
// Protection Rule: Never hardcode UA strings anywhere else in the project.
// =========================================================================
#define UA_WEBVIEW  "Mozilla/5.0 (X11; Ubuntu; Linux x86_64) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/60.5 Safari/605.1.15"
#define UA_CHROME   "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/148.0.0.0 Safari/537.36"
#define UA_FIREFOX  "Mozilla/5.0 (Macintosh; Intel Mac OS X 15.7; rv:150.0) Gecko/20100101 Firefox/150.0"
#define UA_SAFARI   "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/18.0 Safari/605.1.15"
#define UA_EDGE     "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/148.0.0.0 Safari/537.36 Edg/148.0.3967.7"

#define DEFAULT_USER_AGENT  UA_WEBVIEW

// =========================================================================
// Settings Persistence Configuration
// =========================================================================
#define SETTINGS_DB_FILENAME    "settings.sqlite"
#define SETTINGS_KEY_USER_AGENT "user_agent"


#define GOOGLE_OAUTH_BASE "https://accounts.google.com/o/oauth2/v2/auth"
#define OAUTH_PROMPT_SELECT_ACCOUNT "prompt=select_account"

#endif // PROJECT_H
