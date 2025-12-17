#!/bin/bash

# Lightning AI Build Script for Stable Diffusion CLI
# Optimized for H100 GPU (Headless Mode)

set -e

echo "================================"
echo " Building SD CLI for Lightning AI"
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

# Build stable-diffusion.cpp CLI tool
cd stable-diffusion.cpp
rm -rf build
mkdir -p build
cd build

# Configure
echo ""
echo "Configuring with CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DSD_CUBLAS=ON \
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
echo "Executable: stable-diffusion.cpp/build/bin/sd"
echo ""
echo "Next steps:"
echo "1. Download a model: wget https://huggingface.co/runwayml/stable-diffusion-v1-5/resolve/main/v1-5-pruned-emaonly.safetensors -O models/sd-v1-5.safetensors"
echo "2. Generate: ./stable-diffusion.cpp/build/bin/sd -m models/sd-v1-5.safetensors -p 'your prompt' -o output.png"
