# =============================================================================
# meson.build - Meson build definition for github-client
# =============================================================================
#
# This is the Meson build file for github-client.
# It follows CIAO principles: Clean, Intentional, Anti-fragile, Over-engineered.
#
# Features:
#   - Automatic detection of C or C++ sources
#   - Proper dependency handling via pkg-config (GTK+ 3 + WebKit2GTK)
#   - Out-of-source build by default
#   - Install target support
#   - Debug-friendly output directory structure
#
# Recommended usage:
#   meson setup builddir
#   meson compile -C builddir
#   meson install -C builddir
#
# Last updated: April 2026
# =============================================================================

project('github-client',
    'c',
    version: '1.0.0',
    license: 'MIT',
    default_options: [
        'warning_level=3',
        'buildtype=debugoptimized',
        'c_std=c11'
    ]
)

# === FIX: Add this line ===
fs = import('fs')
# ========================

# Dependencies
gtk_dep = dependency('gtk+-3.0', required: true)
webkit_dep = dependency('webkit2gtk-4.1', required: true)

dependencies = [gtk_dep, webkit_dep]

# Source files
src_files = files(
    'src/main/c/main.c',
    'src/main/c/cookies.c',
    'src/main/c/dialog_version.c',
    'src/main/c/load_page.c',
    'src/main/c/menu_bookmark_github.c',
    'src/main/c/menu_exit.c',
    'src/main/c/output.c',
    'src/main/c/url_bar.c'
)

# Check if C++ sources exist (future-proofing)
cpp_files = []
if fs.exists('src/main/cpp/main.cpp')
    cpp_files = files('src/main/cpp/main.cpp')
    add_languages('cpp')
endif

# Build executable
executable('github-client',
    sources: src_files + cpp_files,
    dependencies: dependencies,
    install: true,
    install_dir: get_option('bindir')
)

# Custom install message
meson.add_install_script('sh', '-c', 'echo "github-client installed to ${MESON_INSTALL_PREFIX}/bin/github-client"')