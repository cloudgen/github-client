Here are the updated steps to create a GNOME application for YouTube using Apache Ant, including the installation of Ant via SDKMAN.

# Design Requirements for the GNOME YouTube Application

## Overview
The goal of this project is to create a GNOME application that serves as a simple viewer for YouTube, using C as the programming language and Apache Ant as the build automation tool. This section discusses the design requirements, including folder structure, file breakdowns, usage of header files, and the Ant build script structure.

## Suggested Folder Structure
To maintain an organized project structure, the following folder layout is recommended:

```
yt-client/
├── src/
│   └── main/
│       └── c/
│           ├── cookies.c
│           ├── cookies.h
│           ├── main.c
│           └── project.h
├── target/
├── project.ini
└── build.xml
```

## File Breakdown

### `main.c`
The `main.c` file serves as the entry point for the application. Key functionalities include:
- **Initialization**: Setting up the GTK library and creating the main application window.
- **Web View**: Loading the YouTube homepage using `WebKitWebView`.
- **Cookie Management**: Integrating the cookie management system to handle user data.

### `cookies.c`
The `cookies.c` file handles the creation and management of directories necessary for storing cookies:
- **Directory Creation**: The `create_directories` function checks and creates necessary directories for cookie storage.
- **Storage Initialization**: The `initialize_cookie_storage` function sets the path for cookies and ensures the required directories exist.

### Use of `cookies.h`
The `cookies.h` header file simplifies `cookies.c` by providing function prototypes and including necessary libraries, enhancing modularity and readability.

### Use of `project.h`
The `project.h` file centralizes the management of variables such as project ID, name, and storage paths. This approach allows for easier updates and maintenance.

## Ant Build Script Structure

### Structure of `build.xml`
The `build.xml` file defines the build process and contains several target tasks, detailed below:

```xml
<?xml version="1.0" encoding="utf-8"?>
<project name="yt-client" default="build" basedir=".">
    <!-- Define project properties -->
    <property file="project.ini"/>
    <property name="app.name" value="${PRJ}"/>
    <property name="src.dir" value="src"/>
    <property name="src.c.dir" value="${src.dir}/main/c"/>
    <property name="build.dir" value="target"/>
    <property name="build.debug.dir" value="${build.dir}/debug"/>
    <property name="version" value="1.0"/>
    <property name="author" value="july.wong@gmail.com"/>
    <property name="lib.versions" value="gtk+-3.0 webkit2gtk-4.1"/>
    <property name="project.sh" value="./project.sh"/>
    <property name="required.packages" value="build-essential libwebkit2gtk-4.1-dev gstreamer1.0-plugins-good gstreamer1.0-plugins-ugly"/>
    <property name="avoided.packages" value="gstreamer1.0-plugins-bad"/>
    <property name="runtime.packages" value="libwebkit2gtk-4.1-0"/>

    <!-- Macro for replacing properties in files -->
    <macrodef name="replace-property" taskname="@{taskname}">
        <attribute name="src" />
        <attribute name="dest" default="" />
        <attribute name="replace" default="" />
        <attribute name="with" default="" />
        <sequential>
            <loadresource property="@{dest}">
                <propertyresource name="@{src}" />
                <filterchain>
                    <tokenfilter>
                        <filetokenizer/>
                        <replacestring from="@{replace}" to="@{with}"/>
                    </tokenfilter>
                </filterchain>
            </loadresource>
        </sequential>
    </macrodef>

    <!-- Check if main.c exists -->
    <target name="check-sub-main">
        <available file="${src.c.dir}/main.c" property="sub-main-exists"/>
    </target>

    <!-- Process if main.c exists -->
    <target name="process-main" depends="list-sources, check-sub-main" if="sub-main-exists">
        <replace-property src="app.name" dest="app_name" replace="-" with="_"/>
        <echo file="${src.c.dir}/Makefile.am" append="false"><![CDATA[${app_name}_SOURCES += ${src.c}
]]></echo>
    </target> 

    <!-- Generate Makefile -->
    <target name="generate-Makefile" depends="list-sources">
        <replace-property src="app.name" dest="app_name" replace="-" with="_"/>
        <condition property="flag" value="${app_name}_CFLAGS = $(GTK_CFLAGS)&#10;${app_name}_LDADD = $(GTK_LIBS)" else="">
            <contains string="${lib.versions}" substring="gtk"/>
        </condition>
        <echo file="Makefile.am" append="false"><![CDATA[bin_PROGRAMS = ${app.name}
${app_name}_SOURCES = ${src.c.lists}
${flag}
]]></echo>
    </target>

    <!-- List source files -->
    <target name="list-sources">
        <echo message="Listing all source files in ${src.c.dir}:"/>
        <fileset id="src.lists" dir="${src.c.dir}" includes="**/*.c"/> 
        <property name="tmp-sources" refid="src.lists"/>
        <replace-property src="tmp-sources" dest="tmp-src" replace=";" with=" ${src.c.dir}/"/>        
        <replace-property src="tmp-sources" dest="src.c" replace=";" with=" "/>        
        <property name="src.c.lists" value="${src.c.dir}/${tmp-src}"/>
        <echo message="Source files: ${src.c.lists}"/>
    </target>

    <!-- Generate configure.ac file -->
    <target name="generate-configure">
        <condition property="pkg-check" value="[GTK], " else="">
            <contains string="${lib.versions}" substring="gtk"/>
        </condition>
        <condition property="ac-sub" value="AC_SUBST([GTK_CFLAGS])&#10;AC_SUBST([GTK_LIBS])&#10;" else="...">
            <contains string="${lib.versions}" substring="gtk"/>
        </condition>
        <echo file="configure.ac" append="false"><![CDATA[AC_INIT([${app.name}], [${version}], [${author}])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CC
PKG_CHECK_MODULES(${pkg-check}[${lib.versions}])
${ac-sub}AC_CONFIG_FILES([Makefile])
AC_OUTPUT
]]></echo>
    </target>

    <!-- Prepare build directories -->
    <target name="prepare" depends="generate-Makefile,generate-configure,process-main">
        <mkdir dir="${build.debug.dir}"/> 
    </target>

    <!-- Install required packages -->
    <target name="install-packages" depends="list-sources">
        <echo file="${project.sh}" append="false"><![CDATA[#!/bin/bash
# Export project variables
export PRJ=${app.name}
export SRC=${src.dir}
export SRC_C="${src.c.lists}"
export BUILD=${build.dir}
export BUILD_DEBUG=$BUILD/exe/debug
# Variable name LIB reserved for ./configure
export REQUIRED_LIBS="${lib.versions}"

# Array of prerequisite packages
REQUIRED_PACKAGES="${required.packages}"

# Array of avoided packages
AVOIDED_PACKAGES="${avoided.packages}"

apt_install() {
  RESULT=$(apt list --installed 2>&1 | grep "$2")
  if [ -z "$RESULT" ]; then 
    if [ -z "$1" ]; then
      sudo apt update
    fi
    sudo apt install -y -qq "$2"
    echo "1"
  else 
    echo "$1"
  fi 
}

apt_uninstall() {
  if apt list --installed 2>&1 | grep -q "$1"; then 
    sudo apt purge --autoremove -y -qq "$1"
  fi 
}

install() {
  UPD=0
  echo "$REQUIRED_PACKAGES" | tr ' ' '\n' | while read -r package; do
    if [ ! -z "$package" ]; then
      UPD=$(apt_install $UPD "$package")
    fi
  done

  echo "$AVOIDED_PACKAGES" | tr ' ' '\n' | while read -r package; do
    if [ ! -z "$package" ]; then
      apt_uninstall "$package"
    fi
  done
}

install
]]></echo>
        <chmod file="${project.sh}" perm="755"/>
        <exec executable="${project.sh}"/>
    </target>

    <!-- Build the application -->
    <target name="build-application" depends="prepare">
        <exec executable="autoreconf" failonerror="false">
            <arg value="--install"/>
        </exec>
        <exec executable="./configure"/>
        <exec executable="make"/>
        <move file="${app.name}" tofile="${build.debug.dir}/${app.name}"/>
        <echo message="Please run: ${build.debug.dir}/${app.name}"/>
    </target>

    <!-- Compile the source code -->
    <target name="compile-sources" depends="install-packages,list-sources">
        <exec executable="pkg-config" 
            failonerror="false" 
            outputproperty="include-dirs">
            <arg value="--cflags"/>
            <arg value="--libs"/>
            <arg line="${lib.versions}"/>
        </exec>

        <exec executable="/usr/bin/gcc">
            <arg value="-std=c11" />
            <arg line="${src.c.lists}"/>
            <arg value="-o"/>
            <arg value="${app.name}"/> 
            <arg value="-x"/>
            <arg value="c"/>
            <arg value="-I${src.c.dir}"/> <!-- Include the source directory -->
            <arg line="${include-dirs}"/> <!-- Use pkg-config flags -->
        </exec>
    </target>

    <!-- Move the compiled binary to the exec directory -->
    <target name="move-binary" depends="compile-sources">
        <move file="${app.name}" tofile="${build.debug.dir}/${app.name}"/> <!-- Move to the final location -->
    </target>

    <!-- Clean all generated files -->
    <target name="clean-up">
        <echo message="Cleaning up generated files ...."/>
        <delete dir="autom4te.cache"/>
        <delete dir="${src.c.dir}/.deps"/>
        <delete file="${src.c.dir}/Makefile.am"/>
        <delete dir="${build.dir}"/>
        <delete dir="build"/>
        <delete dir=".gradle"/>
        <delete file="aclocal.m4"/>
        <delete file="config.log"/>
        <delete file="config.status"/>
        <delete file="compile"/>
        <delete file="configure"/>
        <delete file="configure.ac"/>
        <delete file="configure~"/>
        <delete file="install-sh"/>
        <delete file="depcomp"/>
        <delete file="Makefile"/>
        <delete file="Makefile.in"/>
        <delete file="Makefile.am"/>
        <delete file="missing"/>
        <delete file="project.sh"/>
        <delete file="${src.c.dir}/.dirstamp"/>
        <delete dir="${src.c.dir}" includes="*.o"/>
        <delete file="${src.c.dir}/Makefile.in"/>
    </target>

    <!-- Build the application -->
    <target name="build" depends="clean-up, move-binary">
        <echo message="Build completed. You can run the application with the command: ${build.debug.dir}/${app.name}"/>
    </target>

    <target name="base-info">
        <echo message="here ${basedir}"/>
    </target>

    <target name="check-executable">
        <available file="${build.debug.dir}/${app.name}" property="exe-exists"/>
    </target>

    <target name="build-if-not-exists" depends="check-executable" unless="exe-exists">
        <antcall target="build"/>
    </target>

    <!-- Run the application -->
    <target name="run-application" depends="build-if-not-exists">
        <exec executable="${build.debug.dir}/${app.name}"/>
    </target>

    <!-- Create the .deb package -->
    <target name="create-deb-package">
        <mkdir dir="${build.dir}/${app.name}/DEBIAN"/>
        <mkdir dir="${build.dir}/${app.name}/usr/bin"/>
        <mkdir dir="${build.dir}/${app.name}/usr/share/applications"/>

        <copy file="${build.debug.dir}/${app.name}" 
              tofile="${build.dir}/${app.name}/usr/bin/${app.name}"/>
        <chmod file="${build.dir}/${app.name}/usr/bin/${app.name}" perm="755"/>
        <echo file="${build.dir}/${app.name}/DEBIAN/control" append="false"><![CDATA[
Package: ${app.name}
Version: ${version}
Section: utils
Priority: optional
Architecture: amd64
Depends: ${runtime.packages}
Maintainer: ${author}
Description: A simple Youtube Client application.
 A brief description of your application that provides more detail.
]]></echo>

        <echo file="${build.dir}/${app.name}/usr/share/applications/${app.name}.desktop" append="false"><![CDATA[
[Desktop Entry]
Name=Youtube Client
Comment=View your Youtube Client in a simple interface
Exec=${app.name}
Icon=/path/to/icon.png  # Update with your icon path
Terminal=false
Type=Application
Categories=Utility;
]]></echo>

        <exec executable="dpkg-deb" failonerror="true">
            <arg value="--build"/>
            <arg value="${build.dir}/${app.name}"/>
        </exec>
    </target>
</project>
```

## Explanation of Ant Build Targets

### Executable Requirement
The `executable="pkg-config"` in the compile task is necessary to retrieve the compile and link flags for the libraries used in the project. `pkg-config` simplifies the process of managing library dependencies by providing the correct flags for compilation and linking, ensuring that the application can find and use the appropriate libraries.

### Target Descriptions
1. **check-sub-main**: Verifies if `main.c` exists in the source directory. This check ensures that the build process has the required entry point before proceeding.

2. **process-main**: Executes actions if `main.c` exists, such as replacing specific tokens in the `Makefile.am` file to adapt the build configuration based on the project name.

3. **generate-Makefile**: Creates a `Makefile.am` based on the project's source files and library dependencies. This file is crucial for the build process.

4. **list-sources**: Gathers all source files in the specified directory and prepares a list for compilation. It also logs the found files for verification.

5. **generate-configure**: Constructs the `configure.ac` file, which is used to set up the build environment and check for necessary libraries and features.

6. **prepare**: Creates the necessary directories for the build process, including the debug output directory. It ensures the environment is set up for building the application.

7. **install-packages**: Generates a script (`project.sh`) to install required packages on the system, ensuring all dependencies are met for compiling and running the application.

8. **build-application**: Compiles the application using `autoreconf`, `configure`, and `make`, orchestrating the build process to produce the final executable.

9. **compile-sources**: Compiles the source code using `gcc`, with flags obtained from `pkg-config`. This task directly translates source code into an executable.

10. **move-binary**: Moves the compiled executable to the designated output directory, ensuring the final product is organized.

11. **clean-up**: Removes all generated files, directories, and artifacts from previous builds, ensuring a clean slate for future builds.

12. **build**: Orchestrates the overall build process, ensuring that the application is compiled and prepared for execution, while providing feedback on the completion.

13. **base-info**: Displays the base directory of the project, useful for debugging or verifying the project setup.

14. **check-executable**: Checks if the compiled executable exists in the specified directory, helping to manage build dependencies.

15. **build-if-not-exists**: Calls the build target if the executable does not already exist, preventing unnecessary rebuilds.

16. **run-application**: Executes the application if it exists, allowing for easy launching of the built application.

17. **create-deb-package**: Compiles the final application and prepares a Debian package for installation, including necessary metadata and installation scripts.


### Steps to Create a GNOME Application for YouTube Using Apache Ant

1. **Install SDKMAN, Java, and Ant**:
   First, install SDKMAN, which will help you manage Java and Ant versions:
   ```bash
   curl -fsSL https://get.sdkman.io | bash
   source ~/.bashrc
   sdk install java 24.ea.36-opengra
   sdk install ant 1.10.14
   ```

2. **Verify Installation**:
   Ensure that the correct versions of Ant and Java are installed:
   ```bash
   ant -version
   java -version
   ```

   You should see Ant version 1.10.14 and Java version 24.

3. **Create Project Directory Structure**:
   Create the necessary directories for your project:
   ```bash
   mkdir -p yt-client/src/main/c
   cd yt-client
   ```

4. **Write the Application Code**:
    Create a file named `project.h` in the `src/main/c` directory:
    ```c
// project.h
#ifndef PROJECT_H
#define PROJECT_H

#define PROJECT_ID "yt-client"
#define PROJECT_NAME "Youtube Viewer"
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 1
#define START_PAGE "https://youtube.com"

// Define the persistence storage path relative to the home directory
#define PERSISTENCE_STORAGE_PATH ".app/" PROJECT_ID "/cookies"

#endif // PROJECT_H

    ```
    Create a file named `cookies.h` in the `src/main/c` directory:
    ```c
// cookies.h
#ifndef COOKIES_H
#define COOKIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h> // Include errno for error handling

void create_directories(const char *path);
void initialize_cookie_storage(char* full_storage_path);
void test_cookie_storage(char* full_storage_path);

#endif // COOKIES_H
    ```
    Create a file named `cookies.c` in the `src/main/c` directory:
    ```c
// cookies.c
#include "project.h"
#include "cookies.h"

// Function to create directories recursively
void create_directories(const char *path) {
    char *temp_path = strdup(path);
    char *dir = strtok(temp_path, "/");
    char full_path[1024] = "/";

    while (dir != NULL) {
        strcat(full_path, dir);
        strcat(full_path, "/");

        // Create the directory
        if (mkdir(full_path, 0700) && errno != EEXIST) {
            perror("mkdir");
            free(temp_path);
            return;
        }
        dir = strtok(NULL, "/");
    }

    free(temp_path);
}

// Function to initialize cookie storage
void initialize_cookie_storage(char *full_storage_path) {
    const char *home = getenv("HOME");
    if (home == NULL) {
        fprintf(stderr, "HOME environment variable not set.\n");
        return;
    }

    // Ensure the full_storage_path is large enough
    snprintf(full_storage_path, 1024, "%s/%s", home, PERSISTENCE_STORAGE_PATH);
    create_directories(full_storage_path);
}

// Test function
void test_cookie_storage(char *full_storage_path) {
    initialize_cookie_storage(full_storage_path);
    printf("Cookie storage initialized at: %s\n", full_storage_path);
}
    ```
    Create a file named `main.c` in the `src/main/c` directory:
    ```c
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include "cookies.h"
#include "project.h"

int main(int argc, char *argv[]) {
    char full_storage_path[1024];
    char cookie_path[1039];

    gtk_init(&argc, &argv);
    test_cookie_storage(full_storage_path);
    snprintf(cookie_path, 1039, "%s/cookies.sqlite", full_storage_path);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), PROJECT_NAME);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a WebKitWebContext for persistent storage
    WebKitWebContext *web_context = webkit_web_context_new();

    // Get the cookie manager
    WebKitCookieManager *cookie_manager = webkit_web_context_get_cookie_manager(web_context);

    // Set persistent storage for cookies
    webkit_cookie_manager_set_persistent_storage(
        cookie_manager,
        cookie_path,
        WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE
    );

    // Create a WebKitWebView with the web context
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new_with_context(web_context));

    // Load a URI
    webkit_web_view_load_uri(web_view, START_PAGE);

    // Add the web view to the window
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(web_view));

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
    ```

5. **Create the `project.ini` File**:
    Create a `project.ini` file in the project root with the following content:
    ```
PRJ=yt-client
SRC=src
BUILD=target
VERSION=1.0
AUTHOR=july.wong@gmail.com
REQUIRED_PACKAGES=build-essential libwebkit2gtk-4.1-dev gstreamer1.0-plugins-good gstreamer1.0-plugins-ugly
AVOIDED_PACKAGES=gstreamer1.0-plugins-bad
LIB_VERSIONS=gtk+-3.0 webkit2gtk-4.1
    ```
6. **Create the `build.xml` File**:
   Create a `build.xml` file in the project root with the following content:
   ```xml
<?xml version="1.0" encoding="utf-8"?>
<project name="yt-client" default="build" basedir=".">
    <!-- Define project properties -->
    <property file="project.ini"/>

    <!-- Define project properties with defaults -->    
    <property name="app.name" value="${PRJ}"/>
    <property name="src.dir" value="src"/>
    <property name="src.c.dir" value="${src.dir}/main/c"/>
    <property name="build.dir" value="target"/>
    <property name="build.debug.dir" value="${build.dir}/debug"/>
    <property name="version" value="1.0"/>
    <property name="author" value="july.wong@gmail.com"/>
    <property name="lib.versions" value="gtk+-3.0 webkit2gtk-4.1"/>
    <property name="project.sh" value="./project.sh"/>
    <property name="required.packages" value="build-essential libwebkit2gtk-4.1-dev gstreamer1.0-plugins-good gstreamer1.0-plugins-ugly"/>
    <property name="avoided.packages" value="streamer1.0-plugins-bad"/>
    <property name="runtime.packages" value="libwebkit2gtk-4.1-0" />
    
    <macrodef name="replace-property" taskname="@{taskname}">
        <attribute name="src" />
        <attribute name="dest" default="" />
        <attribute name="replace" default="" />
        <attribute name="with" default="" />
        <sequential>
            <loadresource property="@{dest}">
                <propertyresource name="@{src}" />
                <filterchain>
                    <tokenfilter>
                        <filetokenizer/>
                        <replacestring from="@{replace}" to="@{with}"/>
                    </tokenfilter>
                </filterchain>
            </loadresource>
        </sequential>
    </macrodef>

    <target name="check-sub-main">
        <available file="${src.c.dir}/main.c" property="sub-main-exists"/>
    </target>

    <target name="do-if-main" depends="list-sources, check-sub-main" if="sub-main-exists">
        <replace-property src="app.name" dest="app_name" replace="-" with="_"/>
        <echo file="${src.c.dir}/Makefile.am" append="false"><![CDATA[${app_name}_SOURCES += ${src.c}
]]></echo>
    </target> 

    <target name="generate-Makefile" depends="list-sources">
        <replace-property src="app.name" dest="app_name" replace="-" with="_"/>
        <condition property="flag" value="${app_name}_CFLAGS = $(GTK_CFLAGS)&#10;${app_name}_LDADD = $(GTK_LIBS)" else="">
            <contains string="${lib.versions}" substring="gtk"/>
        </condition>
        <echo file="Makefile.am" append="false"><![CDATA[bin_PROGRAMS = ${app.name}
${app_name}_SOURCES = ${src.c.lists}
${flag}
]]></echo>
    </target>

    <target name="list-sources">
        <echo message="Listing all source files in ${src.c.dir}:"/>
        <fileset id="src.lists" dir="${src.c.dir}" includes="**/*.c"/> 
        <property name="tmp-sources" refid="src.lists"/>
        <replace-property src="tmp-sources" dest="tmp-src" replace=";" with=" ${src.c.dir}/"/>        
        <replace-property src="tmp-sources" dest="src.c" replace=";" with=" "/>        
        <property name="src.c.lists" value="${src.c.dir}/${tmp-src}"/>
        <echo message="Source files: ${src.c.lists}"/>
    </target>

    <target name="generate-configure">
        <condition property="pkg-check" value="[GTK], " else="">
            <contains string="${lib.versions}" substring="gtk"/>
        </condition>
        <condition property="ac-sub" value="AC_SUBST([GTK_CFLAGS])&#10;AC_SUBST([GTK_LIBS])&#10;" else="...">
            <contains string="${lib.versions}" substring="gtk"/>
        </condition>
        <echo file="configure.ac" append="false"><![CDATA[AC_INIT([${app.name}], [${version}], [${author}])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CC
PKG_CHECK_MODULES(${pkg-check}[${lib.versions}])
${ac-sub}AC_CONFIG_FILES([Makefile])
AC_OUTPUT
]]></echo>
    </target>

    <!-- Target to prepare directories -->
    <target name="prepare" depends="generate-Makefile,generate-configure,do-if-main">
        <mkdir dir="${build.debug.dir}"/> 
    </target>

    <!-- Target to install required packages -->
    <target name="install" depends="list-sources">
        <echo file="${project.sh}" append="false"><![CDATA[#!/bin/bash
# Export project variables
export PRJ=${app.name}
export SRC=${src.dir}
export SRC_C="${src.c.lists}"
export BUILD=${build.dir}
export BUILD_DEBUG=$BUILD/exe/debug
# Variable name LIB reserved for ./configure
export REQUIRED_LIBS="${lib.versions}"

# Array of prerequisite packages
REQUIRED_PACKAGES="${required.packages}"

# Array of avoided packages
AVOIDED_PACKAGES="${avoided.packages}"

apt_install() {
  RESULT=$(apt list --installed 2>&1 | grep "$2")
  if [ -z "$RESULT" ]; then 
    if [ -z "$1" ]; then
      sudo apt update
    fi
    sudo apt install -y -qq "$2"
    echo "1"
  else 
    echo "$1"
  fi 
}

apt_uninstall() {
  if apt list --installed 2>&1 | grep -q "$1"; then 
    sudo apt purge --autoremove -y -qq "$1"
  fi 
}

install() {
  UPD=0
  echo "$REQUIRED_PACKAGES" | tr ' ' '\n' | while read -r package; do
    if [ ! -z "$package" ]; then
      UPD=$(apt_install $UPD "$package")
    fi
  done

  echo "$AVOIDED_PACKAGES" | tr ' ' '\n' | while read -r package; do
    if [ ! -z "$package" ]; then
      apt_uninstall "$package"
    fi
  done
}

install
]]></echo>
        <chmod file="${project.sh}" perm="755"/>
        <exec executable="${project.sh}"/>
    </target>

    <target name="make" depends="prepare">
        <exec executable="autoreconf" 
            failonerror="false">
            <arg value="--install"/>
        </exec>
        <exec executable="./configure"/>
        <exec executable="make"/>
        <move file="${app.name}" tofile="${build.debug.dir}/${app.name}"/>
        <echo message="Please run: ${build.debug.dir}/${app.name}"/>
    </target>

    <!-- Target to compile the source code -->
    <target name="compile" depends="install,list-sources">
        <exec executable="pkg-config" 
            failonerror="false" 
            outputproperty="include-dirs">
            <arg value="--cflags"/>
            <arg value="--libs"/>
            <arg line="${lib.versions}"/>
        </exec>

        <exec executable="/usr/bin/gcc">
            <arg value="-std=c11" />
            <arg line="${src.c.lists}"/>
            <arg value="-o"/>
            <arg value="${app.name}"/> 
            <arg value="-x"/>
            <arg value="c"/>
            <arg value="-std=c11"/>
            <arg value="-I${src.c.dir}"/> <!-- Include the source directory -->
            <arg line="${include-dirs}"/> <!-- Use pkg-config flags -->
        </exec>
    </target>

    <!-- Target to move the compiled binary to the exec directory -->
    <target name="move" depends="compile">
        <move file="${app.name}" tofile="${build.debug.dir}/${app.name}"/> <!-- Move to the final location -->
    </target>

    <!-- Target to clean all generated files -->
    <target name="clean">
        <echo message="Cleaning up generated files ...."/>
        <delete dir="autom4te.cache"/>
        <delete dir="${src.c.dir}/.deps"/>
        <delete file="${src.c.dir}/Makefile.am"/>
        <delete dir="${build.dir}"/>
        <delete dir="build"/>
        <delete dir=".gradle"/>
        <delete file="aclocal.m4"/>
        <delete file="config.log"/>
        <delete file="config.status"/>
        <delete file="compile"/>
        <delete file="configure"/>
        <delete file="configure.ac"/>
        <delete file="configure~"/>
        <delete file="install-sh"/>
        <delete file="depcomp"/>
        <delete file="Makefile"/>
        <delete file="Makefile.in"/>
        <delete file="Makefile.am"/>
        <delete file="missing"/>
        <delete file="project.sh"/>
        <delete file="${src.c.dir}/.dirstamp"/>
        <delete dir="${src.c.dir}" includes="*.o"/>
        <delete file="${src.c.dir}/Makefile.in"/>
    </target>

    <!-- Target to build the application -->
    <target name="build" depends="clean, move">
        <echo message="Build completed. You can run the application with the command: ${build.debug.dir}/${app.name}"/>
    </target>

    <target name="base">
        <echo message="here ${basedir}"/>
    </target>

    <target name="check-exe">
      <available file="${build.debug.dir}/${app.name}" property="exe-exists"/>
    </target>

    <target name="build-if-not-exists" depends="check-exe" unless="exe-exists">
      <antcall target="build"/>
    </target>

    <!-- Target to run the application -->
    <target name="run" depends="build-if-not-exists">
        <exec executable="${build.debug.dir}/${app.name}"/>
    </target>

    <!-- New target to create the .deb package -->
    <target name="deb">
        <mkdir dir="${build.dir}/${app.name}/DEBIAN"/>
        <mkdir dir="${build.dir}/${app.name}/usr/bin"/>
        <mkdir dir="${build.dir}/${app.name}/usr/share/applications"/>

        <copy file="${build.debug.dir}/${app.name}" 
              tofile="${build.dir}/${app.name}/usr/bin/${app.name}"/>
        <chmod file="${build.dir}/${app.name}/usr/bin/${app.name}" perm="755"/>
        <echo file="${build.dir}/${app.name}/DEBIAN/control" append="false"><![CDATA[
Package: ${app.name}
Version: ${version}
Section: utils
Priority: optional
Architecture: amd64
Depends: ${runtime.packages}
Maintainer: ${author}
Description: A simple Youtube Client application.
 A brief description of your application that provides more detail.
]]></echo>

        <echo file="${build.dir}/${app.name}/usr/share/applications/${app.name}.desktop" append="false"><![CDATA[
[Desktop Entry]
Name=Youtube Client
Comment=View your Youtube Client in a simple interface
Exec=${app.name}
Icon=/path/to/icon.png  # Update with your icon path
Terminal=false
Type=Application
Categories=Utility;
]]></echo>

        <exec executable="dpkg-deb" failonerror="true">
            <arg value="--build"/>
            <arg value="${build.dir}/${app.name}"/>
        </exec>
    </target>
</project>

   ```

7. **Build the Application**:
   Run the following command to build the application:
   ```bash
   ant build
   ```

8. **Run the Application**:
   After building, you can run the application using:
   ```bash
   ant run
   ```

9. **Clean Up**:
   To clean up the generated files, run:
   ```bash
   ant clean
   ```

### Conclusion

The design requirements for the GNOME YouTube application involve careful consideration of file organization, modularity through header files, and a robust Ant build script. By adhering to modern C standards and utilizing external configurations, the project aims to achieve flexibility and maintainability.

You have successfully created a GNOME application that opens YouTube using Apache Ant version 1.10.14 and Java version 24. The application is built from a C source file located at `src/main/c/main.c`, and the `build.xml` file is configured to compile and link the application appropriately.