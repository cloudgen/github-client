# Meson + Ninja Build Steps for github-client

**Meson + Ninja** is the modern, clean, and fast build system preferred for GTK and GNOME applications. It provides out-of-source builds, excellent dependency handling, and is significantly faster and simpler than autotools for this type of project.

This guide follows the defensive CIAO style used throughout the project.

## Prerequisites

Make sure you have installed the required packages:

```bash
./build.sh install
sudo apt install meson ninja-build
```

## Basic Meson Workflow

### 1. Clean previous build artifacts (recommended)

```bash
./build.sh clean
```

This removes stale files (including any previous `builddir`).

### 2. Build with Meson

The `build.sh meson` command handles everything defensively:

```bash
./build.sh meson
```

This will:
- Check for `meson` and `ninja`
- Set up the build directory (`builddir`)
- Configure the project (using the generated `meson.build` logic internally)
- Compile with Ninja
- Copy the resulting binary to the consistent location: `target/exe/debug/github-client`

### 3. Run the application

```bash
./target/exe/debug/github-client
```

## Manual Meson Commands (for advanced users / troubleshooting)

If you prefer to run Meson directly:

```bash
# 1. Configure (create build directory)
meson setup builddir

# 2. Build
meson compile -C builddir
# or: ninja -C builddir

# 3. The binary will be in builddir/github-client
# Copy it to the standard location for consistency:
mkdir -p target/exe/debug
cp builddir/github-client target/exe/debug/github-client 2>/dev/null || true
```

## Reconfiguring / Changing Options

```bash
meson setup --reconfigure builddir
meson configure builddir   # View/edit options
```

## Common Issues & Solutions

- **"meson command not found"** → `sudo apt install meson ninja-build`
- **Dependency errors** (gtk+-3.0 or webkit2gtk-4.1) → Run `./build.sh install` again
- **Stale build** → Run `./build.sh clean` then `./build.sh meson`
- **Permission issues** → Avoid running as root; Meson works best as regular user

## Why Meson?

- Much cleaner and faster than autotools
- Out-of-source builds by design (no pollution of source tree)
- Excellent for GTK/WebKit2GTK projects (used by GNOME itself)
- Simple, readable `meson.build` files
- Consistent with modern Linux desktop development practices

The `build.sh meson` wrapper ensures the output binary lands in the same place as the other methods, maintaining full compatibility with the rest of the project and CIAO tooling.

For general Meson documentation: https://mesonbuild.com/

---

**Last updated:** April 2026

Happy building with Meson!
