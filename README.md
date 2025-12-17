# Stable Diffusion GUI - C++ Implementation

A modern C++ GUI application for generating images with Stable Diffusion models (SD v1.x, v2.x, SDXL).

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)](https://cmake.org/)
[![ImGui](https://img.shields.io/badge/GUI-ImGui-orange.svg)](https://github.com/ocornut/imgui)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

## ✨ Features

### User-Friendly Interface
- 🎨 Clean, modern GUI built with Dear ImGui
- 🖼️ Real-time preview of generated images
- ⚙️ Adjustable generation parameters
- 📊 Progress tracking during generation

### Generation Controls
- ✍️ Text prompts with negative prompts
- 📐 Multiple image sizes (512x512, 768x512, 512x768, custom)
- 🔢 Adjustable steps, CFG scale, and seed
- 🎯 Multiple sampling methods (Euler A, Euler, Heun, DPM2, DPM++ 2M, LCM)
- 📅 Multiple schedulers (Discrete, Karras, Exponential, AYS)

### Model Support
- 📦 Supports `.safetensors`, `.ckpt`, and `.gguf` model formats
- 🤖 SD v1.x, v2.x, and SDXL models
- 📂 Easy model loading through file browser

### Image Management
- 👁️ View generated images in the app
- 💾 Save images as PNG
- 🔄 Automatic image scaling to fit window

## 🚀 Quick Start

### 1. Build the Application

**Easiest Way (Windows):**
```powershell
.\build_and_run.ps1
```

This script will:
- Configure the project with CMake
- Download dependencies (GLFW, ImGui, stb)
- Build the application
- Launch the GUI

**Manual Build:**
```powershell
mkdir build_gui
cd build_gui
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### 2. Download a Model

Download a Stable Diffusion model to the `models/` folder:

```powershell
# Create models directory
mkdir models -ErrorAction SilentlyContinue

# Download SD v1.5 (Recommended - 4GB)
Invoke-WebRequest -Uri "https://huggingface.co/runwayml/stable-diffusion-v1-5/resolve/main/v1-5-pruned-emaonly.safetensors" -OutFile "models\sd-v1-5.safetensors"
```

**Available Models:**
- **SD v1.5** (4GB): Best for starting - [Download](https://huggingface.co/runwayml/stable-diffusion-v1-5)
- **SD v2.1** (5.2GB): Higher quality - [Download](https://huggingface.co/stabilityai/stable-diffusion-2-1)
- **SDXL** (6.9GB): Best quality, slower - [Download](https://huggingface.co/stabilityai/stable-diffusion-xl-base-1.0)

### 3. Run the Application

```powershell
.\build_gui\bin\Release\sd_gui.exe
```

Or simply run the build script again:
```powershell
.\build_and_run.ps1
```

## 📖 Usage Guide

### Loading a Model

1. Click `File` → `Load Model...`
2. Select your model file (`.safetensors`, `.ckpt`, or `.gguf`)
3. Wait for "Model Loaded" status (10-30 seconds)

### Generating Images

1. **Enter Prompt**: Describe what you want to generate
   ```
   a beautiful mountain landscape at sunset, golden hour, highly detailed
   ```

2. **Enter Negative Prompt**: What to avoid
   ```
   blurry, low quality, distorted, deformed
   ```

3. **Choose Size**: Select from presets or enter custom dimensions
   - 512x512 (fastest)
   - 768x512 (landscape)
   - 512x768 (portrait)

4. **Adjust Settings**:
   - **Steps**: 20-30 recommended (higher = better quality, slower)
   - **CFG Scale**: 7-9 recommended (how closely to follow prompt)
   - **Seed**: -1 for random, or specific number for reproducible results

5. **Click Generate**: Watch the progress bar and wait for your image!

### Saving Images

- Click `File` → `Save Image...`
- Choose location and filename
- Images saved as PNG format

## 💡 Tips for Best Results

### Prompt Writing
- **Be specific**: "portrait of a young woman with blue eyes" vs "woman"
- **Add style keywords**: "highly detailed", "4k", "photorealistic", "oil painting"
- **Mention lighting**: "golden hour", "studio lighting", "natural light"
- **Include composition**: "centered", "close-up", "wide angle"

### Example Prompts

**Landscapes:**
```
a serene mountain lake at sunrise, mist over water, pine trees, golden hour lighting, highly detailed, 4k
```

**Portraits:**
```
portrait photo of a woman with curly red hair, natural lighting, professional photography, bokeh background
```

**Fantasy:**
```
magical forest with glowing mushrooms, fairy lights, fireflies, fantasy art, ethereal atmosphere, detailed
```

**Cyberpunk:**
```
futuristic city street at night, neon signs, rain reflections, cyberpunk style, detailed buildings
```

### Negative Prompts
Common negative prompt to avoid quality issues:
```
blurry, low quality, distorted, cartoon, deformed, ugly, bad anatomy, watermark
```

### Recommended Settings

| Use Case | Size | Steps | CFG | Sampler | Scheduler |
|----------|------|-------|-----|---------|-----------|
| Quick test | 512x512 | 15 | 7 | Euler A | Discrete |
| Standard | 512x512 | 25 | 7.5 | DPM++ 2M | Karras |
| High quality | 768x768 | 30 | 8 | DPM++ 2M | Karras |
| Fast (LCM) | 512x512 | 6 | 2 | LCM | Discrete |

## 🛠️ Advanced Options

### GPU Acceleration

Build with CUDA support for NVIDIA GPUs:
```bash
cmake .. -DUSE_CUDA=ON
cmake --build . --config Release
```

Build with Vulkan support:
```bash
cmake .. -DUSE_VULKAN=ON
cmake --build . --config Release
```

### Memory Optimization

For systems with limited RAM:
- Use smaller models (SD v1.5 instead of SDXL)
- Use quantized GGUF models
- Close other applications
- Generate at lower resolutions

## 🔧 Troubleshooting

### Build Issues

**CMake not found:**
```powershell
winget install Kitware.CMake
# Then restart PowerShell
```

**Build fails:**
- Ensure you have Visual Studio or MinGW installed
- Delete `build_gui` folder and rebuild
- Check internet connection (downloads dependencies)

### Runtime Issues

**Model won't load:**
- Check file path is correct
- Ensure enough RAM (8GB minimum, 16GB for SDXL)
- Try a smaller model

**Generation is slow:**
- Reduce image size to 512x512
- Reduce steps to 20
- Use Euler A sampler (faster than DPM++ 2M)
- Consider GPU build if you have compatible GPU

**Out of memory:**
- Use 512x512 resolution
- Close other applications
- Restart the application
- Use quantized GGUF model format

**Poor quality images:**
- Increase steps to 30-40
- Try CFG scale between 7-9
- Improve your prompt (be more specific)
- Try different sampler/scheduler combination
- Check if model loaded correctly

**Application crashes:**
- Check model file isn't corrupted
- Ensure all dependencies built correctly
- Try rebuilding from scratch

## 📁 Project Structure

```
Inference_of_diff/
├── src/
│   ├── main.cpp              # Entry point
│   ├── gui_app.cpp/h         # Main application window
│   ├── sd_generator.cpp/h    # SD generation wrapper
│   └── image_viewer.cpp/h    # Image display and save
├── stable-diffusion.cpp/     # Inference library (submodule)
├── models/                   # Your model files (.safetensors, .gguf)
├── CMakeLists.txt           # Build configuration
├── build_and_run.ps1        # Automated build script
└── README.md                # This file
```

## 📋 System Requirements

### Minimum
- **OS**: Windows 10/11 64-bit
- **CPU**: x64 with AVX2 support
- **RAM**: 8GB (16GB for SDXL)
- **Storage**: 10GB free space
- **GPU**: Optional (CPU mode available)

### Recommended
- **CPU**: Modern multi-core processor (8+ cores)
- **RAM**: 16GB or more
- **GPU**: NVIDIA GPU with 6GB+ VRAM (for GPU mode)
- **Storage**: SSD with 20GB+ free space

## 🚀 Lightning AI / H100 GPU Support

For **100x faster inference** on H100 GPU, see [LIGHTNING_AI.md](LIGHTNING_AI.md) for detailed setup instructions.

### Performance Comparison

| Hardware | SD v1.5 (512x512) | SDXL (1024x1024) |
|----------|-------------------|------------------|
| CPU (AVX2) | 30-60 seconds | 2-5 minutes |
| H100 GPU | **0.5-1 second** ⚡ | **2-3 seconds** ⚡ |

**Speed Improvement: ~50-100x faster with H100!**

## 🔗 Credits

- [stable-diffusion.cpp](https://github.com/leejet/stable-diffusion.cpp) - Inference engine
- [Dear ImGui](https://github.com/ocornut/imgui) - GUI framework
- [GLFW](https://www.glfw.org/) - Window management
- [stb_image_write](https://github.com/nothings/stb) - Image saving

## 📄 License

See LICENSE file for details.

## 🆘 Support

Need help? Check these resources:

1. **Troubleshooting section** above
2. [stable-diffusion.cpp Issues](https://github.com/leejet/stable-diffusion.cpp/issues)
3. Model documentation on [Hugging Face](https://huggingface.co/models)
4. [Lightning AI Documentation](https://lightning.ai/docs) for GPU setup

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

---

**Happy Generating! 🎨✨**

Made with ❤️ using C++ and ImGui. Special thanks to the [stable-diffusion.cpp](https://github.com/leejet/stable-diffusion.cpp) project and its contributors.
