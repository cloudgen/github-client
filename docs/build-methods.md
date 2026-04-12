# Build Methods for github-client

The **github-client** is a lightweight native GNOME desktop application that embeds a WebKit2 web view as a dedicated wrapper for GitHub.

## Project Credits & Background

- **Current Author**: Cloudgen Wong ([@cloudgen](https://github.com/cloudgen)) – Hong Kong
- **CIAO Defensive Programming Principles**: Founded by Cloudgen Wong.  
  This build script and many parts of the project heavily apply **CIAO** (Caution • Intentional • Anti-fragile • Over-engineered) defensive programming practices — emphasizing robustness, edge-case handling, verbose safety checks, and resistance to common AI-induced simplifications.
- **Original Reference**: Heavily inspired and modified from the `pix-client` project by **Wilgat** ([https://github.com/Wilgat/pix-client](https://github.com/Wilgat/pix-client)).  
  The original work provides the core architecture for a family of lightweight GTK + WebKit2GTK desktop wrappers (including persistent cookies, URL bar, bookmarks, and multi-build-system support).

This project takes the solid foundation from Wilgat’s `pix-client` and significantly enhances it with CIAO principles for greater reliability across different shells, environments, and build scenarios.

All documentation is located in the `docs/` directory.

## Overview of Build Methods

| Method              | Build Tool               | Speed     | Best For                              | Complexity | Recommended For                  |
|---------------------|--------------------------|-----------|---------------------------------------|------------|----------------------------------|
| **Basic / Simple**  | Direct `gcc`             | Fastest   | Quick testing & development           | Very Low   | Beginners, daily dev             |
| **Meson + Ninja**   | Meson + Ninja            | Very Fast | Modern GTK/WebKit2 apps, clean builds | Low        | Most users (new default)         |
| **GNU Make**        | Autotools                | Medium    | Standard Linux-style builds           | Medium     | Traditional Linux workflows      |
| **Gradle**          | Gradle + cpp plugin      | Medium    | Modern builds, CI/CD                  | Medium     | Power users, automation          |
| **Ant**             | Apache Ant               | Slow      | Legacy or Java-mixed projects         | High       | Specific legacy needs            |

All methods ultimately produce the executable at:  
**`target/exe/debug/github-client`** (Basic, GNU Make, Meson)  
or **`target/debug/github-client`** (Gradle)

---

## 1. Basic / Simple GCC Build (Recommended for beginners / quick tests)

**Fastest and simplest method** — compiles directly with `gcc` without any build system overhead.

**Main commands:**
```bash
./build.sh clean
./build.sh install
./build.sh build
./target/exe/debug/github-client
```

→ Detailed guide: **[docs/basic-steps.md](basic-steps.md)**

---

## 2. Meson + Ninja Build (Modern & Recommended)

Uses the current standard build system for GNOME/GTK applications. Clean, fast, out-of-source builds, excellent dependency handling.

**Main commands:**
```bash
./build.sh clean && ./build.sh install
./build.sh meson
./target/exe/debug/github-client
```

→ Detailed guide: **[docs/meson-steps.md](meson-steps.md)**

---

## 3. GNU Make / Autotools Build (Most Standard / Traditional)

Uses the classic `./configure && make` workflow with proper autotools support.

**Main commands:**
```bash
./build.sh clean
./build.sh install
./build.sh make
./target/exe/debug/github-client
```

→ Detailed guide: **[docs/gnu-make-steps.md](gnu-make-steps.md)**

---

## 4. Gradle Build (Modern & Flexible)

Uses Gradle with the `cpp-application` plugin. Automatically detects C or C++ sources.

**Main commands:**
```bash
./gradlew clean
./gradlew installDependencies
./gradlew build
./gradlew run
```

→ Detailed guide: **[docs/gradle-steps.md](gradle-steps.md)**

---

## 5. Ant Build (Legacy Support)

Kept for compatibility with older Ant-based setups.

**Main command:**
```bash
./build.sh ant
```

→ Detailed guide: **[docs/ant-steps.md](ant-steps.md)**

---

## Recommended Workflows

### Quick Development (Basic/Simple)
```bash
./build.sh clean && ./build.sh install && ./build.sh build && ./target/exe/debug/github-client
```

### Modern Professional Build (Meson — Recommended)
```bash
./build.sh clean && ./build.sh install && ./build.sh meson
```

### Standard Professional Build (GNU Make)
```bash
./build.sh clean && ./build.sh install && ./build.sh make
```

### Full Clean Rebuild
```bash
./build.sh clean
./build.sh install
./build.sh meson   # or ./build.sh make
```

## Quick Command Reference

| Goal                        | Command                                      |
|-----------------------------|----------------------------------------------|
| Install dependencies        | `./build.sh install`                         |
| Clean everything            | `./build.sh clean`                           |
| Simple GCC build            | `./build.sh build`                           |
| Meson build                 | `./build.sh meson`                           |
| Autotools build             | `./build.sh make`                            |
| Gradle build + run          | `./gradlew build run`                        |
| Run the program             | `./target/exe/debug/github-client`           |

## Which Method Should You Use?

- **Just want it to work quickly?** → **Basic/Simple** (`./build.sh build`)
- **Want the best modern experience?** → **Meson + Ninja** (`./build.sh meson`)
- **Want a standard Linux experience?** → **GNU Make** (`./build.sh make`)
- **Prefer modern tools / CI/CD?** → **Gradle**
- **Working with legacy Ant?** → **Ant**

All five methods are maintained and produce a working binary.

For detailed instructions, troubleshooting, and advanced options, see the individual guides in the `docs/` folder.

---

**Last updated:** April 2026

Happy building!
