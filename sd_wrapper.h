/*
 * Real Model Wrapper for stable-diffusion.cpp
 * 
 * This wrapper provides a simplified C++ interface to the actual
 * stable-diffusion.cpp library for real model inference.
 */

#ifndef SD_WRAPPER_H
#define SD_WRAPPER_H

#include <string>
#include <memory>

// Forward declarations
struct sd_ctx_t;
struct sd_image_t;

namespace sd_real {

// Configuration for model loading
struct ModelConfig {
    std::string model_path;
    std::string vae_path;        // Optional separate VAE
    std::string lora_path;       // Optional LoRA
    int n_threads = 4;
    bool vae_decode_only = true;
    bool flash_attention = false;
};

// Generation parameters
struct GenerationParams {
    std::string prompt;
    std::string negative_prompt;
    int width = 512;
    int height = 512;
    int steps = 20;
    float cfg_scale = 7.5f;
    int64_t seed = -1;
    int batch_count = 1;
    std::string sample_method = "euler_a";  // euler_a, euler, heun, dpm2, dpmpp2m
    std::string scheduler = "karras";       // discrete, karras, exponential
    
    // Image-to-image params
    std::string init_image_path;
    float strength = 0.75f;
};

// Image result
struct Image {
    int width = 0;
    int height = 0;
    int channels = 0;
    std::unique_ptr<uint8_t[]> data;
    
    bool save_png(const std::string& filename) const;
    bool save_jpg(const std::string& filename, int quality = 90) const;
};

// Main inference class
class StableDiffusion {
public:
    StableDiffusion();
    ~StableDiffusion();
    
    // Initialize with model
    bool load_model(const ModelConfig& config);
    
    // Generate image from text
    std::unique_ptr<Image> generate(const GenerationParams& params);
    
    // Image-to-image
    std::unique_ptr<Image> img2img(const GenerationParams& params);
    
    // Get model info
    std::string get_model_info() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace sd_real

#endif // SD_WRAPPER_H
