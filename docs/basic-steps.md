# Basic Build Steps for github-client

This document provides the simplest way to build and run the **github-client** project using the provided `./build.sh` script.

The **simple build** (also called direct GCC build) is the fastest and most straightforward method — it compiles the C sources directly with `gcc` without using autotools or Gradle.

## Quick Start (Recommended One-liner)

```bash
./build.sh clean && ./build.sh build && ./target/exe/debug/github-client
```

This cleans previous artifacts, builds the project, and immediately runs the application.

## Step-by-Step Instructions

### 1. Install Dependencies (First time only)

```bash
./build.sh install
```

This installs the required system packages:
- `build-essential`
- `libwebkit2gtk-4.1-dev`

### 2. Clean Previous Build (Recommended)

```bash
./build.sh clean
```

This removes old binaries, object files, and build directories to ensure a fresh build.

### 3. Perform Simple GCC Build

```bash
./build.sh build
```

or equivalently:

```bash
./build.sh simple
```

What this does:
- Compiles all `.c` files listed in the script (`main.c`, `cookies.c`, etc.)
- Links against GTK+ 3.0 and WebKit2GTK using `pkg-config`
- Places the executable at: **`target/exe/debug/github-client`**

### 4. Run the Program

```bash
./target/exe/debug/github-client
```

## Full Basic Workflow

```bash
# Recommended full sequence
./build.sh clean
./build.sh install
./build.sh build
./target/exe/debug/github-client
```

## Available Commands for Simple Build

| Command       | Description                              |
|---------------|------------------------------------------|
| `install`     | Install system dependencies              |
| `build`       | Simple GCC build (recommended)           |
| `simple`      | Same as `build`                          |
| `clean`       | Remove all build artifacts               |
| `help`        | Show help message                        |

### Useful Options

- Quiet mode:  
  ```bash
  ./build.sh build --quiet
  ```

- JSON output (for scripts):  
  ```bash
  ./build.sh build --json
  ```

## Summary of Commands

```bash
./build.sh install          # Install dependencies
./build.sh clean            # Clean everything
./build.sh build            # Compile with gcc
./target/exe/debug/github-client   # Run the app
```

## Notes

- The simple build is ideal for quick development and testing.
- The final binary is always placed in `target/exe/debug/github-client`.
- If you prefer a more "professional" build with `./configure` and `make`, see `gnu-make-steps.md`.
- For Gradle-based builds, see `gradle-steps.md`.

Last updated: April 2026
