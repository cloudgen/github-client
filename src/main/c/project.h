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
#define VERSION_PATCH   0

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

#endif // PROJECT_H