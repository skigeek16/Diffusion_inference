# Stable Diffusion GUI - Lightning AI Setup

Quick setup guide for running on Lightning AI with H100 GPU.

## Quick Start on Lightning AI

### 1. Clone the Repository
```bash
git clone <your-repo-url>
cd Inference_of_diff
```

### 2. Install Dependencies
```bash
# Update system
sudo apt-get update

# Install CMake if needed
sudo apt-get install -y cmake build-essential

# Clone stable-diffusion.cpp
git clone --recursive https://github.com/leejet/stable-diffusion.cpp
```

### 3. Build with CUDA Support
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_CUDA=ON
cmake --build . -j$(nproc)
```

### 4. Download a Model
```bash
mkdir -p models
cd models

# SD v1.5 (4GB)
wget https://huggingface.co/runwayml/stable-diffusion-v1-5/resolve/main/v1-5-pruned-emaonly.safetensors

# Or SDXL (7GB) for better quality
# wget https://huggingface.co/stabilityai/stable-diffusion-xl-base-1.0/resolve/main/sd_xl_base_1.0.safetensors
```

### 5. Run the GUI
```bash
cd ..
./build/bin/Release/sd_gui
```

## Performance on H100

The H100 GPU will provide significantly faster inference:
- **SD v1.5**: ~0.5-1s per image (512x512, 25 steps)
- **SDXL**: ~2-3s per image (1024x1024, 25 steps)

Compare to CPU: 30-60s per image

## Environment Variables for GPU

```bash
# Enable CUDA optimizations
export CUDA_VISIBLE_DEVICES=0
export GGML_CUDA_NO_PINNED=1
```

## Jupyter Notebook Example

```python
import subprocess
import os

# Set model path
model_path = "models/sd-v1-5.safetensors"

# Run generation
result = subprocess.run([
    "./build/bin/Release/sd_gui",
    "--model", model_path,
    "--prompt", "a beautiful landscape",
    "--output", "output.png",
    "--steps", "25",
    "--cfg-scale", "7.5"
], capture_output=True, text=True)

print(result.stdout)
```

## Tips for Lightning AI

1. **Use GPU**: Make sure to select a GPU instance (H100 preferred)
2. **Storage**: Download models to `/teamspace/studios/this_studio/models`
3. **Display**: Use X11 forwarding or save images to file
4. **Batch Processing**: Generate multiple images in parallel

## Troubleshooting

### Display Issues
If you can't open the GUI window:
```bash
# Save to file instead
export DISPLAY=:0
# Or use headless mode (if implemented)
```

### Out of Memory
```bash
# Use smaller model or reduce batch size
# Monitor with: nvidia-smi
```

### Build Errors
```bash
# Make sure CUDA toolkit is installed
nvcc --version

# Update CMake if needed
pip install cmake --upgrade
```
