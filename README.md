# github-client

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/Version-1.0.0-blue.svg)](CIAO-PRINCIPLES.md)
[![CIAO](https://img.shields.io/badge/Philosophy-CIAO%20(Caution%20%E2%80%A2%20Intentional%20%E2%80%A2%20Anti--fragile%20%E2%80%A2%20Over--engineered)-purple.svg)](https://github.com/cloudgen/ciao)
[![GTK](https://img.shields.io/badge/UI-GTK%203-brightgreen.svg)](https://gtk.org)
[![WebKit2](https://img.shields.io/badge/Engine-WebKit2GTK-orange.svg)](https://webkitgtk.org)

**github-client** is a lightweight native GNOME desktop application that embeds a **WebKit2** web view, designed as a clean, dedicated wrapper for **GitHub**.

Instead of opening GitHub in a full web browser, you get a focused, native Linux window with persistent login, quick bookmarks, URL bar, and a simple interface.

This project is built using [CIAO](https://github.com/cloudgen/ciao) (Caution • Intentional • Anti-fragile • Over-engineered).

---

## Features

- Embedded WebKit2GTK browser engine
- Persistent cookie storage (`~/.app/github-client/cookies/`) — stay logged in across sessions
- Smart URL bar with automatic updating
- Quick bookmarks for GitHub and common pages
- Clean menu bar (File → Bookmark / Exit, About → Show Version)
- Lightweight and fast (much lighter than Electron-based apps)
- Flexible build system supporting **Simple GCC**, **GNU Make (autotools)**, **Gradle**, **Ant**, and **Meson + Ninja**
- Fully compatible with **SyncPrjs** project synchronization tool

---

## Project Background & Credits

- **Author**: Cloudgen Wong ([@cloudgen](https://github.com/cloudgen)) – Hong Kong
- **CIAO Principles**: This project is built using **CIAO (Caution • Intentional • Anti-fragile • Over-engineered)** — a defensive programming methodology founded by Cloudgen Wong.  
  [CIAO](https://github.com/cloudgen/ciao) emphasizes writing robust, thoughtful, and deliberately over-engineered code that resists breakage and handles edge cases gracefully.  
  See **[CIAO-PRINCIPLES.md](CIAO-PRINCIPLES.md)** and the official CIAO repository: [https://github.com/cloudgen/ciao](https://github.com/cloudgen/ciao)
- **Original Reference**: Heavily inspired and modified from the `pix-client` project by **Wilgat** ([https://github.com/Wilgat/pix-client](https://github.com/Wilgat/pix-client)).
- **SyncPrjs Compatibility**: Fully compatible with **SyncPrjs** by Wilgat ([https://github.com/Wilgat/SyncPrjs](https://github.com/Wilgat/SyncPrjs)).

Many core ideas (modular C structure, cookie persistence, multi-build-system support, and GTK + WebKit2 architecture) come from Wilgat’s family of “*-client” wrappers.

---

## Quick Start

### 1. Install dependencies (Ubuntu / Debian)

```bash
./build.sh install
```

**For Meson** (recommended modern method), also install Meson + Ninja:

```bash
sudo apt install meson ninja-build
```

### 2. Build the application

**Recommended (fastest & simplest):**

```bash
./build.sh clean && ./build.sh build
```

**Alternative build methods:**

```bash
# Meson + Ninja (modern, clean, recommended for GTK apps)
./build.sh meson

# GNU Make / Autotools
./build.sh make

# Gradle
./gradlew clean build

# Ant (legacy)
./build.sh ant
```

The executable will be placed at:

```
target/exe/debug/github-client
```

### 3. Run the application

```bash
./target/exe/debug/github-client
```

---

## Usage

- Enter or paste a URL in the address bar and press **Go**
- Use **File → Bookmark** for quick access to GitHub pages
- **About → Show Version** displays build and version information
- All cookies and login sessions are automatically persisted

---

## Build Methods

Detailed instructions for all **five** build systems are available in the [`docs/`](docs/) folder:

- **[docs/build-methods.md](docs/build-methods.md)** — Overview of all build methods
- **[docs/basic-steps.md](docs/basic-steps.md)** — Simple GCC build (recommended for quick testing)
- **[docs/gnu-make-steps.md](docs/gnu-make-steps.md)** — GNU Make / Autotools
- **[docs/gradle-steps.md](docs/gradle-steps.md)** — Gradle build
- **[docs/ant-steps.md](docs/ant-steps.md)** — Ant build (legacy)
- **[docs/meson-steps.md](docs/meson-steps.md)** — **Meson + Ninja** (new modern method)

---

## Project Structure

```bash
.
├── README.md
├── CIAO-PRINCIPLES.md        # CIAO philosophy applied in this project
├── build.sh                  # Main defensive build script (CIAO edition)
├── build.gradle
├── build.xml
├── project.ini
├── LICENSE.md
├── SECURITY.md
├── docs/                     # Detailed build guides
│   ├── build-methods.md
│   ├── basic-steps.md
│   ├── gnu-make-steps.md
│   ├── gradle-steps.md
│   ├── ant-steps.md
│   └── meson-steps.md        # New: Meson build guide
└── src/
    └── main/
        └── c/                # Modular C source files
```

---

## Technologies

- **GTK+ 3** — Native UI toolkit
- **WebKit2GTK** — Modern web rendering engine
- **C** — Clean, modular implementation
- Multiple build systems for maximum flexibility (including **Meson**, the current standard for GNOME/GTK projects)

---

## Related Projects

- **Wilgat’s pix-client**: https://github.com/Wilgat/pix-client (original inspiration)
- **Wilgat’s SyncPrjs**: https://github.com/Wilgat/SyncPrjs (project synchronization tool – fully compatible)

---

**Last updated:** April 2026

Happy coding and browsing GitHub natively!
