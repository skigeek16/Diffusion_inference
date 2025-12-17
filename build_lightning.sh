#!/bin/bash

# Lightning AI Build Script for Stable Diffusion GUI
# Optimized for H100 GPU

set -e

echo "================================"
echo " Building SD GUI for Lightning AI"
echo "================================"
echo ""

# Check for CUDA
if ! command -v nvcc &> /dev/null; then
    echo "Warning: CUDA not found. Building CPU version..."
    USE_CUDA=OFF
else
    echo "✓ CUDA found: $(nvcc --version | grep release | awk '{print $5}' | sed 's/,//')"
    USE_CUDA=ON
fi

# Clone stable-diffusion.cpp if needed
if [ ! -d "stable-diffusion.cpp" ]; then
    echo ""
    echo "Cloning stable-diffusion.cpp..."
    git clone --recursive https://github.com/leejet/stable-diffusion.cpp
fi

# Create build directory
rm -rf build
mkdir -p build
cd build

# Configure
echo ""
echo "Configuring with CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DUSE_CUDA=$USE_CUDA \
    -DGGML_CUDA=ON

# Build
echo ""
echo "Building (this may take a few minutes)..."
cmake --build . --config Release -j$(nproc)

echo ""
echo "================================"
echo " Build Complete!"
echo "================================"
echo ""
echo "Executable: build/bin/Release/sd_gui"
echo ""
echo "Next steps:"
echo "1. Download a model to models/"
echo "2. Run: ./build/bin/Release/sd_gui"
