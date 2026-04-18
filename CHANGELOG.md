# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.4] - 2026-04-18


### Added
- Persistent User-Agent settings using `settings.sqlite` (stored alongside cookies).
- User-Agent selected at startup now correctly displays in the UI label (previously showed default Chrome until changed).
- `webkit_setup.c/h` module — Single Point of Truth for all WebKitWebContext, cookie, and User-Agent initialization.
- `ui_window_github.c/h` and `ui_menu_github.c/h` modules for better separation of concerns.
- Strong CIAO defensive protection comments on critical functions (`create_main_window()`, `build_main_menu_bar()`, etc.).
- `PROJECT_DESCRIPTION` constant in `project.h` (Single Source of Truth).

### Changed
- `main.c` is now pure orchestration (very clean and short).
- `add_url_bar_user_agent()` now accepts an `initial_ua` parameter to respect persisted settings.
- Enhanced protection rules to preserve GitHub-specific UI modules when forking to other projects.
- Updated version dialog to use centralized `PROJECT_DESCRIPTION`.

### Fixed
- UI label under URL bar now shows correct persisted User-Agent on application startup.
- Duplicate loading of User-Agent settings at startup.

---

## [1.0.3] - 2026-04-18

### Added
- Full User-Agent spoofing menu (Chrome, Firefox, Safari, Edge) with Chrome as default.
- Persistent cookie storage using SQLite.
- Advanced output system with `--quiet`, `--json`, and `DEBUG=1` support.
- Generic bookmark system + GitHub-specific bookmarks.
- Real-time URL bar synchronization (`notify::uri` + `load-changed` fallback).
- Enter key support in URL bar.

### Changed
- Migrated to strong CIAO Defensive Programming style across the entire codebase.
- All User-Agent strings centralized in `project.h`.

---

## [1.0.0] - 2026-04-14

### Added
- Initial release of lightweight native GitHub client using GTK3 + WebKit2GTK.
- Basic URL bar and navigation.
- Defensive coding foundation (CIAO principles).

---

## How to Maintain This File

When releasing a new version:
1. Move `[Unreleased]` content into a new version header with today's date.
2. Add a fresh `[Unreleased]` section at the top.
3. Use the standard categories: **Added**, **Changed**, **Fixed**, etc.

---

**Project**: [cloudgen/github-client](https://github.com/cloudgen/github-client)  
**Philosophy**: Built with strict CIAO Defensive Programming Principles

---

Made with ❤️ and extreme caution.