# GNU Make / Autotools Build Steps for github-client

This document explains how to build the **github-client** project using the provided defensive build script `./build.sh`.

The script supports multiple build methods. This guide focuses on the **recommended GNU Autotools path** (`make` / `autotools` / `gnumake`), which generates proper `configure` and `Makefile` files.

## Quick Start (One-liner)

```bash
./build.sh install && ./build.sh make
```

This installs dependencies (if needed) and performs a full autotools build.

## Available Commands

| Command          | Description                                      | When to use                          |
|------------------|--------------------------------------------------|--------------------------------------|
| `install`        | Install required system dependencies via apt    | First time / missing packages        |
| `make`           | Full autotools build (recommended)              | Normal development builds            |
| `build` / `simple` | Simple direct gcc build (no autotools)         | Quick tests, when autotools fails    |
| `clean`          | Thorough cleanup of all generated files         | Before re-building from scratch      |
| `help`           | Show usage help                                 | Anytime                              |

Options:
- `--quiet` or `-q` — Suppress non-error messages
- `--json` — Output machine-readable JSON (implies quiet)

## Step-by-Step: Using the Autotools Build (`./build.sh make`)

1. **Install Dependencies** (only needed once or when packages are missing)

   ```bash
   ./build.sh install
   ```

   This installs:
   - `build-essential`
   - `libwebkit2gtk-4.1-dev`
   - and other packages required for GTK+ 3.0 + WebKit2GTK

2. **Clean Previous Build Artifacts** (recommended before a fresh build)

   ```bash
   ./build.sh clean
   ```

   This removes all generated files including:
   - `configure.ac`, `Makefile.am`
   - `configure`, `config.h.in`, `Makefile`
   - autotools helpers (`compile`, `depcomp`, `install-sh`, etc.)
   - build directory (`target/`)

3. **Build with Autotools**

   ```bash
   ./build.sh make
   ```

   What happens internally:
   - Generates `configure.ac` and `Makefile.am`
   - Generates `config.h.in` (required by `AC_CONFIG_HEADERS`)
   - Runs `aclocal`, `automake`, `autoreconf --install`
   - Runs `./configure`
   - Runs `make`

   On success, the binary is placed at:  
   **`target/exe/debug/github-client`**

4. **Run the Program**

   ```bash
   ./target/exe/debug/github-client
   ```

   Or for convenience:

   ```bash
   ./build.sh make && ./target/exe/debug/github-client
   ```

## Alternative: Simple GCC Build (no autotools)

If the autotools path fails or you want something faster:

```bash
./build.sh clean
./build.sh build        # or ./build.sh simple
```

The binary will still be placed at **`target/exe/debug/github-client`**.

## Full Clean Rebuild Cycle (Recommended for Development)

```bash
./build.sh clean
./build.sh install      # safe to run multiple times
./build.sh make
./target/exe/debug/github-client
```

## Common Issues & Solutions

- **"required file 'config.h.in' not found"**  
  This should no longer occur. Run `./build.sh clean` and try again.

- **Missing packages / pkg-config errors**  
  Run `./build.sh install` again.

- **Permission errors**  
  Use `sudo` only when running `install`. Never run the full build as root.

- **Want to see less output?**  
  Use `./build.sh make --quiet`

## Notes

- The `make` command is the most robust path because it uses proper Autotools infrastructure.
- All generated files are cleaned by `./build.sh clean` to ensure a pristine state on the next build.
- The binary name uses the safe version (`github_client` internally for Makefile variables, but the final executable is named `github-client`).

For more details on the build script internals, see comments in `build.sh`.

Last updated: April 2026
