/*
 * Real Model Wrapper Implementation
 * Links against the actual stable-diffusion.cpp library
 */

#include "sd_wrapper.h"
#include "stable-diffusion.h"  // From stable-diffusion.cpp
#include <iostream>
#include <cstring>
#include <stdexcept>

// STB Image Write for saving PNG/JPG
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace sd_real {

// Private implementation
class StableDiffusion::Impl {
public:
    sd_ctx_t* ctx = nullptr;
    ModelConfig config;
    
    ~Impl() {
        if (ctx) {
            free_sd_ctx(ctx);
        }
    }
};

StableDiffusion::StableDiffusion() : impl_(std::make_unique<Impl>()) {}

StableDiffusion::~StableDiffusion() = default;

bool StableDiffusion::load_model(const ModelConfig& config) {
    impl_->config = config;
    
    // Initialize context parameters
    sd_ctx_params_t params;
    sd_ctx_params_init(&params);
    
    params.model_path = config.model_path.c_str();
    params.n_threads = config.n_threads;
    params.vae_decode_only = config.vae_decode_only;
    
    if (!config.vae_path.empty()) {
        params.vae_path = config.vae_path.c_str();
    }
    
    params.diffusion_flash_attn = config.flash_attention;
    
    // Create context
    impl_->ctx = new_sd_ctx(&params);
    if (!impl_->ctx) {
        std::cerr << "Failed to create SD context" << std::endl;
        return false;
    }
    
    std::cout << "Model loaded successfully: " << config.model_path << std::endl;
    return true;
}

std::unique_ptr<Image> StableDiffusion::generate(const GenerationParams& params) {
    if (!impl_->ctx) {
        throw std::runtime_error("Model not loaded");
    }
    
    std::cout << "\n=== Generating Image ===" << std::endl;
    std::cout << "Prompt: " << params.prompt << std::endl;
    std::cout << "Size: " << params.width << "x" << params.height << std::endl;
    std::cout << "Steps: " << params.steps << std::endl;
    std::cout << "CFG: " << params.cfg_scale << std::endl;
    
    // Setup generation parameters
    sd_img_gen_params_t gen_params;
    sd_img_gen_params_init(&gen_params);
    
    gen_params.prompt = params.prompt.c_str();
    gen_params.negative_prompt = params.negative_prompt.c_str();
    gen_params.width = params.width;
    gen_params.height = params.height;
    gen_params.seed = params.seed;
    gen_params.batch_count = params.batch_count;
    
    // Sample parameters
    gen_params.sample_params.sample_steps = params.steps;
    gen_params.sample_params.guidance.txt_cfg = params.cfg_scale;
    
    // Set sampling method
    if (params.sample_method == "euler_a") {
        gen_params.sample_params.sample_method = EULER_A_SAMPLE_METHOD;
    } else if (params.sample_method == "euler") {
        gen_params.sample_params.sample_method = EULER_SAMPLE_METHOD;
    } else if (params.sample_method == "heun") {
        gen_params.sample_params.sample_method = HEUN_SAMPLE_METHOD;
    } else if (params.sample_method == "dpm2") {
        gen_params.sample_params.sample_method = DPM2_SAMPLE_METHOD;
    } else if (params.sample_method == "dpmpp2m") {
        gen_params.sample_params.sample_method = DPMPP2M_SAMPLE_METHOD;
    }
    
    // Set scheduler
    if (params.scheduler == "karras") {
        gen_params.sample_params.scheduler = KARRAS;
    } else if (params.scheduler == "discrete") {
        gen_params.sample_params.scheduler = DISCRETE;
    } else if (params.scheduler == "exponential") {
        gen_params.sample_params.scheduler = EXPONENTIAL;
    }
    
    // Generate
    sd_image_t* result = generate_image(impl_->ctx, &gen_params);
    
    if (!result) {
        throw std::runtime_error("Generation failed");
    }
    
    // Convert to our Image type
    auto img = std::make_unique<Image>();
    img->width = result->width;
    img->height = result->height;
    img->channels = result->channel;
    
    size_t data_size = img->width * img->height * img->channels;
    img->data = std::make_unique<uint8_t[]>(data_size);
    std::memcpy(img->data.get(), result->data, data_size);
    
    // Free original result
    free(result->data);
    free(result);
    
    std::cout << "Generation complete!" << std::endl;
    return img;
}

std::unique_ptr<Image> StableDiffusion::img2img(const GenerationParams& params) {
    if (!impl_->ctx) {
        throw std::runtime_error("Model not loaded");
    }
    
    if (params.init_image_path.empty()) {
        throw std::runtime_error("init_image_path required for img2img");
    }
    
    // Load init image using stb_image or similar
    // Then call generate_image with init_image set
    
    // For now, delegate to generate with modifications
    return generate(params);
}

std::string StableDiffusion::get_model_info() const {
    if (!impl_->ctx) {
        return "No model loaded";
    }
    return "Model: " + impl_->config.model_path;
}

bool Image::save_png(const std::string& filename) const {
    if (!data || width == 0 || height == 0) {
        std::cerr << "Invalid image data" << std::endl;
        return false;
    }
    
    int result = stbi_write_png(filename.c_str(), width, height, channels, 
                                 data.get(), width * channels);
    
    if (result) {
        std::cout << "Image saved: " << filename << std::endl;
        return true;
    }
    
    std::cerr << "Failed to save PNG: " << filename << std::endl;
    return false;
}

bool Image::save_jpg(const std::string& filename, int quality) const {
    if (!data || width == 0 || height == 0) {
        std::cerr << "Invalid image data" << std::endl;
        return false;
    }
    
    int result = stbi_write_jpg(filename.c_str(), width, height, channels,
                                 data.get(), quality);
    
    if (result) {
        std::cout << "Image saved: " << filename << std::endl;
        return true;
    }
    
    std::cerr << "Failed to save JPG: " << filename << std::endl;
    return false;
}

} // namespace sd_real
