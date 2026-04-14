Here's the **updated README.md** with a new dedicated section on **"The Benefits of Using CIAO Defensive Programming Principles"**, plus more prominent and natural links to https://github.com/cloudgen/ciao.

```markdown
# github-client

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/Version-1.0.1-blue.svg)](https://github.com/cloudgen/github-client/releases)
[![CIAO](https://img.shields.io/badge/Philosophy-CIAO%20(Caution%20%E2%80%A2%20Intentional%20%E2%80%A2%20Anti--fragile%20%E2%80%A2%20Over--engineered)-purple.svg)](https://github.com/cloudgen/ciao)
[![GTK](https://img.shields.io/badge/UI-GTK%203-brightgreen.svg)](https://gtk.org)
[![WebKit2](https://img.shields.io/badge/Engine-WebKit2GTK-orange.svg)](https://webkitgtk.org)

**github-client** is a lightweight native GNOME desktop application that embeds a **WebKit2** web view, designed as a clean, dedicated wrapper for **GitHub**.

Instead of opening GitHub in a full web browser, you get a focused, native Linux window with persistent login, smart URL bar (supports Enter key), quick bookmarks, and minimal distractions.

This project is built using the strict **[CIAO](https://github.com/cloudgen/ciao)** defensive programming philosophy.

---

## Features

- Embedded WebKit2GTK browser engine
- Persistent cookie storage (`~/.app/github-client/cookies/`) — stay logged in across sessions
- Smart URL bar with real-time updating and **Enter key support** (same as clicking Go)
- Quick bookmarks for GitHub and common pages
- Clean menu bar (File → Bookmark / Exit, About → Show Version)
- Lightweight and fast (much lighter than Electron-based apps)
- Multiple build system support: Meson+Ninja (recommended), GNU Make, Gradle, Ant, and simple GCC
- Fully compatible with **SyncPrjs** project synchronization tool

---

## The Benefits of Using CIAO Defensive Programming Principles

This project follows the **[CIAO](https://github.com/cloudgen/ciao)** methodology — **Caution • Intentionality • Anti-fragility • Over-engineering** — a set of 18 defensive programming principles developed to create robust, long-lasting software.

### Why CIAO Matters Here:

- **Anti-fragile code**: The application survives harsh environments, unexpected inputs, and future modifications without breaking.
- **Resistance to "helpful" changes**: Heavy defensive comments and "DO NOT MODIFY" blocks protect critical functions (like URL bar signal handling) from being accidentally simplified or broken by AI assistants or future maintainers.
- **Clear intent**: Every major function has explicit purpose, protection rules, and links to CIAO principles, making the codebase self-documenting and maintainable over years.
- **Fewer runtime crashes**: Strict validation (e.g., `GTK_IS_ENTRY` checks) prevented Gtk-CRITICAL errors that previously occurred when signals were mismatched.
- **Better long-term reliability**: By deliberately over-engineering safety nets (multiple signals, backups, fallbacks), the app remains stable even as WebKitGTK or GTK evolves.

You can read the full **[18 CIAO Principles](https://github.com/cloudgen/ciao)** and see how they are applied in real projects in the official repository:  
→ **[https://github.com/cloudgen/ciao](https://github.com/cloudgen/ciao)**

Many core patterns in this project (defensive callbacks, signal protection, multi-build support, and persistent storage) directly come from applying CIAO rules.

---

## Project Background & Credits

- **Author**: Cloudgen Wong ([@cloudgen](https://github.com/cloudgen)) – Hong Kong
- **Philosophy**: Built with strict **CIAO (Caution • Intentional • Anti-fragile • Over-engineered)** defensive principles.  
  See **[CIAO-PRINCIPLES.md](CIAO-PRINCIPLES.md)** and the official repository: [https://github.com/cloudgen/ciao](https://github.com/cloudgen/ciao)
- **Similar Project**:  
  **[youtube-client](https://github.com/cloudgen/youtube-client)** — A similar lightweight native YouTube client using the same CIAO architecture, defensive patterns, and multi-build-system approach.
- **Original Inspiration**: Heavily based on Wilgat’s family of “*-client” projects.
- **SyncPrjs Compatibility**: Fully compatible with **[SyncPrjs](https://github.com/Wilgat/SyncPrjs)** by Wilgat.

---

## Quick Start

### 1. Install dependencies (Ubuntu / Debian)

```bash
./build.sh install
```

For Meson (recommended):

```bash
sudo apt install meson ninja-build
```

### 2. Build the application

**Recommended (fastest & cleanest):**

```bash
./build.sh clean && ./build.sh build
```

**Other build methods:**

```bash
# Meson + Ninja (modern, recommended for GTK apps)
./build.sh meson

# GNU Make / Autotools
./build.sh make

# Gradle
./gradlew clean build

# Ant (legacy)
./build.sh ant
```

The executable will be available at:

```
target/exe/debug/github-client
```

### 3. Run

```bash
./target/exe/debug/github-client
```

---

## Usage

- Type or paste a URL in the address bar and press **Enter** or click **Go**
- Use **File → Bookmark** for quick access to GitHub pages
- **About → Show Version** shows build and version information
- Cookies and login sessions are automatically persisted

---

## Build Methods

Detailed guides for all supported build systems are available in the [`docs/`](docs/) folder:

- **[docs/build-methods.md](docs/build-methods.md)** — Overview
- **[docs/meson-steps.md](docs/meson-steps.md)** — Meson + Ninja (recommended)
- **[docs/basic-steps.md](docs/basic-steps.md)** — Simple GCC
- **[docs/gnu-make-steps.md](docs/gnu-make-steps.md)** — GNU Make / Autotools
- **[docs/gradle-steps.md](docs/gradle-steps.md)** — Gradle
- **[docs/ant-steps.md](docs/ant-steps.md)** — Ant (legacy)

---

## Project Structure

```bash
.
├── README.md
├── CIAO-PRINCIPLES.md
├── build.sh
├── build.gradle
├── build.xml
├── project.ini
├── LICENSE
├── SECURITY.md
├── CHANGELOG.md
├── docs/
│   └── *.md          # Build guides
└── src/
    └── main/
        └── c/        # Modular C source files
```

---

## Technologies

- **GTK+ 3** — Native UI toolkit
- **WebKit2GTK** — Modern web rendering engine
- **C** — Clean, modular, defensive implementation
- Multi-build-system support for maximum flexibility

---

## Related Projects

- **[youtube-client](https://github.com/cloudgen/youtube-client)** — Similar lightweight native YouTube client using the same CIAO architecture.
- **Wilgat’s pix-client**: https://github.com/Wilgat/pix-client (original inspiration)
- **Wilgat’s SyncPrjs**: https://github.com/Wilgat/SyncPrjs (project synchronization tool – fully compatible)

---

**Last updated:** April 2026

Made with ❤️ and extreme caution.

Happy coding and native GitHub browsing!
