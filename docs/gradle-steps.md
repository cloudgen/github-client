# Gradle Build Steps for github-client

This document explains how to build and run the **github-client** project using **Gradle** with the provided `build.gradle` configuration.

The Gradle build is designed to:
- Automatically detect whether your sources are in C (`src/main/c`) or C++ (`src/main/cpp`)
- Use `pkg-config` for GTK+ 3.0 / WebKit2GTK dependencies
- Generate necessary autotools files (`configure.ac`, `Makefile.am`) when needed
- Place the final binary in `target/debug/github-client`

## Prerequisites

Make sure you have the following installed:
- Gradle (version 6+ recommended)
- `pkg-config`
- Development packages for GTK+ and WebKit2GTK

You can install dependencies using the project's own helper:

```bash
./build.sh install
```

or directly via Gradle:

```bash
./gradlew installDependencies
```

## Basic Usage

### 1. Install Dependencies

```bash
./gradlew installDependencies
```

This task:
- Creates or updates `project.sh` if missing
- Installs required system packages (`build-essential`, `libwebkit2gtk-4.1-dev`, etc.)

### 2. Clean Previous Build

```bash
./gradlew clean
```

This removes:
- Generated autotools files (`configure.ac`, `Makefile.am`, `configure`, etc.)
- Build artifacts in `target/`
- Object files and temporary directories

### 3. Build the Project

```bash
./gradlew build
```

or equivalently:

```bash
./gradlew assemble
```

This will:
- Run `installDependencies` (if not already done)
- Detect C or C++ sources
- Compile using the appropriate compiler (`gcc` or `g++`)
- Link against GTK+ / WebKit2GTK
- Place the executable at: **`target/debug/github-client`**

### 4. Run the Application

```bash
./gradlew run
```

Or directly:

```bash
./target/debug/github-client
```

## Full Recommended Workflow

```bash
# Clean everything first (recommended for fresh builds)
./gradlew clean

# Install system dependencies
./gradlew installDependencies

# Build the project
./gradlew build

# Run the application
./gradlew run
```

Or as a one-liner:

```bash
./gradlew clean installDependencies build run
```

## Available Gradle Tasks

| Task                    | Description                                      |
|-------------------------|--------------------------------------------------|
| `build` / `assemble`    | Full build (compiles + links)                    |
| `run`                   | Build (if needed) and execute the binary         |
| `clean`                 | Remove all generated files and build artifacts   |
| `installDependencies`   | Install system packages via apt                  |
| `make`                  | Traditional autotools-style build (autoreconf + configure + make) |
| `tasks`                 | List all available tasks                         |

You can see all tasks with:

```bash
./gradlew tasks
```

## Common Commands Summary

```bash
# Quick build + run
./gradlew build run

# Clean rebuild
./gradlew clean build run

# Only install dependencies
./gradlew installDependencies

# Only run (assumes binary exists)
./gradlew run

# Traditional make via Gradle wrapper
./gradlew make
```

## Output Locations

- **Binary**: `target/debug/github-client`
- **Build directory**: `target/`
- **Debug build**: `target/debug/`

## Notes

- The build automatically switches between C and C++ mode based on the presence of `src/main/cpp/main.cpp`.
- All compiler and linker flags are derived from `pkg-config` for GTK/WebKit.
- The `make` task inside Gradle performs the full `autoreconf → configure → make` sequence for users who prefer the traditional autotools workflow.
- The Gradle `clean` task is more comprehensive than the simple one in `build.sh` for this setup.

For comparison with other build methods, see:
- `gnu-make-steps.md` — for `./build.sh make`
- Root `build.sh` — for simple gcc build

Last updated: April 2026
