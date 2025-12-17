#pragma once

#include "sd_generator.h"
#include "image_viewer.h"
#include <GLFW/glfw3.h>
#include <memory>
#include <mutex>
#include <vector>

class GUIApp {
public:
    GUIApp();
    ~GUIApp();

    bool initialize();
    void run();
    void shutdown();

private:
    void renderUI();
    void renderMenuBar();
    void renderGenerationPanel();
    void renderImagePanel();
    void renderProgressPanel();
    
    void onGenerate();
    void onSaveImage();
    void onLoadModel();
    
    GLFWwindow* window_ = nullptr;
    std::unique_ptr<SDGenerator> generator_;
    std::unique_ptr<ImageViewer> image_viewer_;
    
    // UI state
    char prompt_buffer_[1024] = "a beautiful landscape with mountains and lake, sunset, highly detailed";
    char negative_buffer_[512] = "blurry, low quality, distorted";
    int width_ = 512;
    int height_ = 512;
    int steps_ = 25;
    float cfg_scale_ = 7.5f;
    int seed_ = -1;
    int sampler_idx_ = 0;
    int scheduler_idx_ = 1; // Karras
    
    // Status
    bool is_generating_ = false;
    int current_step_ = 0;
    int total_steps_ = 0;
    char status_text_[256] = "Ready";
    char model_path_[512] = "";
    bool model_loaded_ = false;
    
    // Pending image data from background thread
    struct PendingImage {
        std::vector<uint8_t> data;
        int width = 0;
        int height = 0;
        int channels = 0;
        bool ready = false;
    };
    PendingImage pending_image_;
    std::mutex pending_image_mutex_;
    
    // Settings
    int n_threads_ = 4;
};
