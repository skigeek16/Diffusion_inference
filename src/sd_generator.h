#pragma once

#include "stable-diffusion.h"
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <atomic>
#include <thread>

class SDGenerator {
public:
    struct GenerationParams {
        std::string prompt = "a beautiful landscape";
        std::string negative_prompt = "blurry, low quality";
        int width = 512;
        int height = 512;
        int steps = 25;
        float cfg_scale = 7.5f;
        int64_t seed = -1;
        sample_method_t sample_method = EULER_A_SAMPLE_METHOD;
        scheduler_t scheduler = DISCRETE_SCHEDULER;
    };

    struct ImageResult {
        int width = 0;
        int height = 0;
        int channels = 0;
        std::vector<uint8_t> data;
        bool valid = false;
    };

    using ProgressCallback = std::function<void(int step, int total_steps, const std::string& status)>;

    SDGenerator();
    ~SDGenerator();

    bool loadModel(const std::string& model_path, int n_threads = 4);
    void unloadModel();
    bool isModelLoaded() const { return model_loaded_; }
    
    void generateAsync(const GenerationParams& params, ProgressCallback progress_cb);
    void cancelGeneration();
    bool isGenerating() const { return is_generating_; }
    
    ImageResult getLastResult();
    std::string getLastError() const { return last_error_; }
    std::string getModelInfo() const { return model_info_; }

private:
    void generateThread(GenerationParams params, ProgressCallback progress_cb);
    
    sd_ctx_t* ctx_ = nullptr;
    bool model_loaded_ = false;
    std::atomic<bool> is_generating_{false};
    std::atomic<bool> should_cancel_{false};
    std::string last_error_;
    std::string model_info_;
    ImageResult last_result_;
    std::unique_ptr<std::thread> gen_thread_;
};
