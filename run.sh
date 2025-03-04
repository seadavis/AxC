#!/bin/bash

# Define paths
BUILD_DIR="build"
BIN_DIR="$BUILD_DIR/bin"
EXECUTABLE="$BIN_DIR/AxC"

# Function to clean CMake build files
clean_build() {
    echo "Cleaning CMake build files..."
    rm -rf "$BUILD_DIR/CMakeCache.txt" "$BUILD_DIR/CMakeFiles" "$BUILD_DIR/Makefile" \
           "$BUILD_DIR/cmake_install.cmake" "$BIN_DIR" "$BUILD_DIR/lib"
    echo "Build directory cleaned."
}

# Function to configure and build the project
build_project() {
    echo "Configuring and building the project..."
    cmake -DCMAKE_BUILD_TYPE=Debug -S . -B "$BUILD_DIR" && cmake --build "$BUILD_DIR"
}

# Parse arguments
RUN_ONLY=false
CLEAN=false
BUILD_ONLY=false
POSITIONAL_ARGS=()

while [[ $# -gt 0 ]]; do
    case $1 in
        -r ) RUN_ONLY=true; shift ;;   # Skip CMake build step
        -c ) CLEAN=true; shift ;;      # Clean build directory
        -b ) BUILD_ONLY=true; shift ;;      # Clean build directory
        -- ) shift; break ;;           # Stop argument parsing
        -* ) echo "Usage: $0 [-r] (run only) [-c] (clean) [-- <args to executable>]"; exit 1 ;;
        * ) POSITIONAL_ARGS+=("$1"); shift ;;  # Store extra arguments
    esac
done

# Clean build files if requested
if $CLEAN; then
    clean_build
fi

# Build the project unless -r is specified
if ! $RUN_ONLY; then
    build_project
fi

# Ensure the executable exists before running
if [[ ! -f "$EXECUTABLE" ]]; then
    echo "Error: Executable not found! Please build the project first."
    exit 1
fi

if ! $BUILD_ONLY; then
    # Run the executable with passed arguments
    echo "Running $EXECUTABLE ${POSITIONAL_ARGS[*]}..."
    "$EXECUTABLE" "${POSITIONAL_ARGS[@]}"
fi
