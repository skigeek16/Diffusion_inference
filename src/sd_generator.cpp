#include "sd_generator.h"
#include <ctime>
#include <cstring>
#include <iostream>

SDGenerator::SDGenerator() {}

SDGenerator::~SDGenerator() {
    cancelGeneration();
    if (gen_thread_ && gen_thread_->joinable()) {
        gen_thread_->join();
    }
    unloadModel();
}

bool SDGenerator::loadModel(const std::string& model_path, int n_threads) {
    if (model_loaded_) {
        unloadModel();
    }

    sd_ctx_params_t params;
    sd_ctx_params_init(&params);
    
    params.model_path = model_path.c_str();
    params.n_threads = n_threads;
    params.vae_decode_only = true;
    params.diffusion_flash_attn = true;
    
    ctx_ = new_sd_ctx(&params);
    if (!ctx_) {
        last_error_ = "Failed to load model: " + model_path;
        return false;
    }
    
    model_loaded_ = true;
    model_info_ = "Model: " + model_path;
    last_error_.clear();
    
    return true;
}

void SDGenerator::unloadModel() {
    if (ctx_) {
        free_sd_ctx(ctx_);
        ctx_ = nullptr;
    }
    model_loaded_ = false;
}

void SDGenerator::generateAsync(const GenerationParams& params, ProgressCallback progress_cb) {
    if (is_generating_) {
        last_error_ = "Generation already in progress";
        return;
    }
    
    if (!model_loaded_) {
        last_error_ = "No model loaded";
        return;
    }
    
    if (gen_thread_ && gen_thread_->joinable()) {
        gen_thread_->join();
    }
    
    should_cancel_ = false;
    gen_thread_ = std::make_unique<std::thread>(
        &SDGenerator::generateThread, this, params, progress_cb);
}

void SDGenerator::cancelGeneration() {
    should_cancel_ = true;
    if (gen_thread_ && gen_thread_->joinable()) {
        gen_thread_->join();
    }
}

void SDGenerator::generateThread(GenerationParams params, ProgressCallback progress_cb) {
    is_generating_ = true;
    last_error_.clear();
    
    try {
        if (progress_cb) {
            progress_cb(0, params.steps, "Initializing...");
        }
        
        // Setup generation parameters
        sd_img_gen_params_t gen_params;
        sd_img_gen_params_init(&gen_params);
        
        gen_params.prompt = params.prompt.c_str();
        gen_params.negative_prompt = params.negative_prompt.c_str();
        gen_params.width = params.width;
        gen_params.height = params.height;
        gen_params.seed = params.seed < 0 ? time(nullptr) : params.seed;
        gen_params.batch_count = 1;
        
        gen_params.sample_params.sample_steps = params.steps;
        gen_params.sample_params.guidance.txt_cfg = params.cfg_scale;
        gen_params.sample_params.sample_method = params.sample_method;
        gen_params.sample_params.scheduler = params.scheduler;
        
        if (progress_cb) {
            progress_cb(0, params.steps, "Generating...");
        }
        
        // Generate image
        sd_image_t* result = generate_image(ctx_, &gen_params);
        
        if (should_cancel_) {
            if (result) {
                free(result->data);
                free(result);
            }
            last_error_ = "Generation cancelled";
            is_generating_ = false;
            return;
        }
        
        if (!result) {
            last_error_ = "Generation failed";
            is_generating_ = false;
            return;
        }
        
        // Store result
        last_result_.width = result->width;
        last_result_.height = result->height;
        last_result_.channels = result->channel;
        last_result_.data.assign(
            result->data,
            result->data + (result->width * result->height * result->channel)
        );
        last_result_.valid = true;
        
        // Free original result
        free(result->data);
        free(result);
        
        if (progress_cb) {
            progress_cb(params.steps, params.steps, "Complete!");
        }
        
    } catch (const std::exception& e) {
        last_error_ = std::string("Exception: ") + e.what();
    }
    
    is_generating_ = false;
}

SDGenerator::ImageResult SDGenerator::getLastResult() {
    return last_result_;
}
