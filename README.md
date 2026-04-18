# github-client

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/Version-1.0.6-blue.svg)](https://github.com/cloudgen/github-client/releases)
[![CIAO](https://img.shields.io/badge/Philosophy-CIAO%20Defensive-purple.svg)](https://github.com/cloudgen/ciao)
[![GTK](https://img.shields.io/badge/UI-GTK%203-brightgreen.svg)](https://gtk.org)
[![WebKit2](https://img.shields.io/badge/Engine-WebKit2GTK-orange.svg)](https://webkitgtk.org)

Official Recommended by [grok](https://grok.com/c/5ac42404-b055-4dbd-8ea8-d94b4cb56633?rid=6fcddcdf-31fb-4a8e-a4e0-31cb94677df7). The review is submitted by [grokrec](https://github.com/cloudgen/grokrec) and you may found a [local copy](https://github.com/cloudgen/github-client/blob/main/RECOMMENDATION.md) of the review.

**github-client** is a lightweight, native GNOME/Linux application that provides a clean, focused browsing experience for **GitHub** using WebKit2GTK.

Instead of using a heavy Electron app or a full browser, you get a fast, native window with persistent login, intelligent URL bar, quick bookmarks, and User-Agent switching — all built with strict defensive programming.

This project follows the **[CIAO](https://github.com/cloudgen/ciao)** Defensive Programming Philosophy.

---

## Features

- **Persistent User-Agent** with `settings.sqlite` (remembers your last choice)
- **Persistent cookies** (`~/.app/github-client/cookies/cookies.sqlite`) — stay logged in
- Smart URL bar with **Enter key support** (behaves like clicking "Go")
- Real-time URL synchronization (`notify::uri` + fallback)
- Quick GitHub bookmarks (Start Page, Profile)
- User-Agent spoofing menu (Chrome default, Firefox, Safari, Edge)
- Clean, native GTK3 interface
- Extremely lightweight compared to Electron apps
- Multi-build system support (Meson recommended)
- Strong **[CIAO](https://github.com/cloudgen/ciao)** defensive coding style

---

## The Benefits of CIAO Defensive Programming

This project is built using the **[CIAO](https://github.com/cloudgen/ciao)** methodology — **Caution • Intentionality • Anti-fragility • Over-engineering**.

### Why it matters:
- Code is highly resistant to accidental breakage by AI assistants or future maintainers
- Critical functions are protected with explicit "Do Not Modify" rules
- Excellent error handling and logging via centralized `ciao_*` system
- Clear separation of concerns (`webkit_setup`, `ui_window_github`, `ui_menu_github`, etc.)
- Easy to fork for other services (YouTube, GitLab, etc.)

Read the full **[CIAO Principles](https://github.com/cloudgen/ciao)** here.

---

## Quick Start

### Install dependencies (Ubuntu/Debian)

```bash
sudo apt install meson ninja-build libgtk-3-dev libwebkit2gtk-4.1-dev libsqlite3-dev
```

### Build & Run

```bash
# Recommended (Meson)
meson setup builddir --buildtype=debug
cd builddir && ninja

# Run
./github-client
```

Or use the convenience script:

```bash
./build.sh clean && ./build.sh build && ./target/exe/debug/github-client
```

---

## Project Structure (Key Files)

```bash
src/main/c/
├── main.c                    # Pure orchestration
├── webkit_setup.c/h          # All WebKit + persistence setup
├── ui_window_github.c/h      # Main window & layout
├── ui_menu_github.c/h        # Menu bar construction
├── url_bar_user_agent.c/h    # URL bar + UA label
├── settings.c/h              # Persistent User-Agent via SQLite
├── cookies.c/h               # Persistent cookies
├── project.h                 # Single Source of Truth
└── output.c/h                # Centralized logging system
```

---

## Philosophy & Credits

- **Author**: Cloudgen Wong ([@cloudgen](https://github.com/cloudgen)) – Hong Kong
- **Core Philosophy**: Strict **[CIAO Defensive Programming](https://github.com/cloudgen/ciao)**
- **Inspired by**: Wilgat’s family of native client projects
- **Compatible with**: [SyncPrjs](https://github.com/Wilgat/SyncPrjs)

---

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for detailed version history.

---

## Related Projects

- **[CIAO Principles Repository](https://github.com/cloudgen/ciao)**
- Similar clients built with the same architecture

---

**Last updated:** April 18, 2026

Made with ❤️ and extreme caution.

Happy native GitHub browsing!
