#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

BUILD_TYPE="Release" # Change to "Debug" if you need debugging symbols

echo "======================================"
echo " Starting Build Process..."
echo "======================================"

# Step 1: Install dependencies using Conan
# cmake_layout() in conanfile.py handles the output folder automatically
# (generates into build/<build_type>/generators/)
echo "📦 Installing dependencies via Conan..."
conan install . --build=missing -s build_type=$BUILD_TYPE

# Step 2: Configure the project with CMake using the Conan-generated preset
echo "⚙️ Configuring CMake..."
PRESET_NAME="conan-$(echo $BUILD_TYPE | tr '[:upper:]' '[:lower:]')"
cmake --preset $PRESET_NAME

# Step 3: Build the executable
echo "🔨 Building the executable..."
cmake --build --preset $PRESET_NAME

echo "======================================"
echo "✅ Build Completed Successfully!"
echo "======================================"

echo "🚀 Launching game..."
./build/$BUILD_TYPE/blocks
