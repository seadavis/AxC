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
    cmake -S . -B "$BUILD_DIR" && cmake --build "$BUILD_DIR"
}

# Parse arguments
RUN_ONLY=false
CLEAN=false

while getopts ":rc" opt; do
    case ${opt} in
        r ) RUN_ONLY=true ;;
        c ) CLEAN=true ;;
        * ) echo "Usage: $0 [-r] (run only) [-c] (clean build directory)"; exit 1 ;;
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

# Run the executable
echo "Running $EXECUTABLE..."
"$EXECUTABLE"
