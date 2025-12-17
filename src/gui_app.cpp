#include "gui_app.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <mutex>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <iostream>
#include <cstring>

#if defined(_WIN32)
#include <windows.h>
#include <commdlg.h>
#endif

GUIApp::GUIApp() 
    : generator_(std::make_unique<SDGenerator>())
    , image_viewer_(std::make_unique<ImageViewer>()) {
}

GUIApp::~GUIApp() {
    shutdown();
}

bool GUIApp::initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(1400, 900, "Stable Diffusion GUI", nullptr, nullptr);
    if (!window_) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize OpenGL loader" << std::endl;
        return false;
    }

    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return true;
}

void GUIApp::run() {
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();
        
        // Check for pending image from background thread (load on main GL thread)
        {
            std::lock_guard<std::mutex> lock(pending_image_mutex_);
            if (pending_image_.ready) {
                printf("Main thread: Loading pending image to OpenGL texture\n");
                image_viewer_->loadImage(pending_image_.data.data(), 
                                        pending_image_.width, 
                                        pending_image_.height, 
                                        pending_image_.channels);
                pending_image_.ready = false;
                pending_image_.data.clear();
            }
        }

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderUI();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window_, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window_);
    }
}

void GUIApp::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

void GUIApp::renderUI() {
    renderMenuBar();
    
    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(450, 880), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Generation Settings", nullptr, ImGuiWindowFlags_NoCollapse);
    renderGenerationPanel();
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(460, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(940, 880), ImGuiCond_FirstUseEver);
    
    ImGui::Begin("Generated Image", nullptr, ImGuiWindowFlags_NoCollapse);
    renderImagePanel();
    ImGui::End();

    if (is_generating_) {
        ImGui::SetNextWindowPos(ImVec2(460, 750), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(940, 150), ImGuiCond_Always);
        ImGui::Begin("Progress", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
        renderProgressPanel();
        ImGui::End();
    }
}

void GUIApp::renderMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load Model...")) {
                onLoadModel();
            }
            if (ImGui::MenuItem("Save Image...", nullptr, false, image_viewer_->hasImage())) {
                onSaveImage();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                glfwSetWindowShouldClose(window_, GLFW_TRUE);
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                // Show about dialog
            }
            ImGui::EndMenu();
        }
        
        ImGui::SameLine(ImGui::GetWindowWidth() - 400);
        if (model_loaded_) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Model Loaded");
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No Model Loaded");
        }
        
        ImGui::EndMainMenuBar();
    }
}

void GUIApp::renderGenerationPanel() {
    ImGui::Text("Model:");
    ImGui::InputText("##modelpath", model_path_, sizeof(model_path_));
    ImGui::SameLine();
    if (ImGui::Button("Browse...")) {
        onLoadModel();
    }
    
    if (model_loaded_) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Model Ready");
    }
    
    ImGui::Separator();
    
    ImGui::Text("Prompt:");
    ImGui::InputTextMultiline("##prompt", prompt_buffer_, sizeof(prompt_buffer_), ImVec2(-1, 100));
    
    ImGui::Text("Negative Prompt:");
    ImGui::InputTextMultiline("##negative", negative_buffer_, sizeof(negative_buffer_), ImVec2(-1, 60));
    
    ImGui::Separator();
    
    ImGui::Text("Size:");
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("Width", &width_, 64, 128);
    ImGui::SetNextItemWidth(100);
    ImGui::InputInt("Height", &height_, 64, 128);
    
    if (ImGui::Button("512x512")) { width_ = height_ = 512; }
    ImGui::SameLine();
    if (ImGui::Button("768x512")) { width_ = 768; height_ = 512; }
    ImGui::SameLine();
    if (ImGui::Button("512x768")) { width_ = 512; height_ = 768; }
    
    ImGui::Separator();
    
    ImGui::SliderInt("Steps", &steps_, 10, 50);
    ImGui::SliderFloat("CFG Scale", &cfg_scale_, 1.0f, 20.0f);
    ImGui::InputInt("Seed (-1 = random)", &seed_);
    
    const char* samplers[] = { "Euler A", "Euler", "Heun", "DPM2", "DPM++ 2M", "LCM" };
    ImGui::Combo("Sampler", &sampler_idx_, samplers, IM_ARRAYSIZE(samplers));
    
    const char* schedulers[] = { "Discrete", "Karras", "Exponential", "AYS" };
    ImGui::Combo("Scheduler", &scheduler_idx_, schedulers, IM_ARRAYSIZE(schedulers));
    
    ImGui::Separator();
    
    ImGui::InputInt("Threads", &n_threads_, 1, 4);
    
    ImGui::Separator();
    
    bool can_generate = model_loaded_ && !is_generating_;
    
    if (!can_generate) {
        ImGui::BeginDisabled();
    }
    
    if (ImGui::Button("Generate", ImVec2(-1, 50))) {
        onGenerate();
    }
    
    if (!can_generate) {
        ImGui::EndDisabled();
    }
    
    if (is_generating_ && ImGui::Button("Cancel", ImVec2(-1, 30))) {
        generator_->cancelGeneration();
    }
}

void GUIApp::renderImagePanel() {
    if (image_viewer_->hasImage()) {
        ImVec2 available = ImGui::GetContentRegionAvail();
        
        float img_w = (float)image_viewer_->getWidth();
        float img_h = (float)image_viewer_->getHeight();
        float scale = std::min(available.x / img_w, available.y / img_h);
        
        ImVec2 display_size(img_w * scale, img_h * scale);
        
        ImGui::Image(
            (ImTextureID)(intptr_t)image_viewer_->getTextureID(),
            display_size
        );
        
        ImGui::Text("Image Size: %dx%d", image_viewer_->getWidth(), image_viewer_->getHeight());
        
    } else {
        ImGui::TextWrapped("No image generated yet.\n\nLoad a model and click 'Generate' to create an image.");
    }
}

void GUIApp::renderProgressPanel() {
    float progress = total_steps_ > 0 ? (float)current_step_ / total_steps_ : 0.0f;
    
    ImGui::Text("Status: %s", status_text_);
    ImGui::ProgressBar(progress, ImVec2(-1, 30));
    ImGui::Text("Step %d / %d (is_generating: %d)", current_step_, total_steps_, is_generating_);
}

void GUIApp::onGenerate() {
    if (!model_loaded_ || is_generating_) return;
    
    SDGenerator::GenerationParams params;
    params.prompt = prompt_buffer_;
    params.negative_prompt = negative_buffer_;
    params.width = width_;
    params.height = height_;
    params.steps = steps_;
    params.cfg_scale = cfg_scale_;
    params.seed = seed_;
    
    // Map sampler
    sample_method_t methods[] = {
        EULER_A_SAMPLE_METHOD, EULER_SAMPLE_METHOD, HEUN_SAMPLE_METHOD,
        DPM2_SAMPLE_METHOD, DPMPP2M_SAMPLE_METHOD, LCM_SAMPLE_METHOD
    };
    params.sample_method = methods[sampler_idx_];
    
    // Map scheduler
    scheduler_t schedulers[] = { DISCRETE_SCHEDULER, KARRAS_SCHEDULER, EXPONENTIAL_SCHEDULER, AYS_SCHEDULER };
    params.scheduler = schedulers[scheduler_idx_];
    
    is_generating_ = true;
    total_steps_ = steps_;
    current_step_ = 0;
    strcpy(status_text_, "Starting generation...");
    
    generator_->generateAsync(params, [this](int step, int total, const std::string& status) {
        printf("Progress: step %d/%d - %s\n", step, total, status.c_str());
        current_step_ = step;
        total_steps_ = total;
        strncpy(status_text_, status.c_str(), sizeof(status_text_) - 1);
        
        if (step >= total) {
            auto result = generator_->getLastResult();
            if (result.valid) {
                printf("Generation complete! Image size: %dx%d, channels: %d\n", 
                       result.width, result.height, result.channels);
                
                // Store image data for main thread to load
                std::lock_guard<std::mutex> lock(pending_image_mutex_);
                pending_image_.data = result.data;
                pending_image_.width = result.width;
                pending_image_.height = result.height;
                pending_image_.channels = result.channels;
                pending_image_.ready = true;
                printf("Image queued for main thread loading\n");
            } else {
                printf("Generation result invalid!\n");
            }
            is_generating_ = false;
        }
    });
}

void GUIApp::onSaveImage() {
#if defined(_WIN32)
    OPENFILENAMEA ofn;
    char filename[MAX_PATH] = "";
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = glfwGetWin32Window(window_);
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrFilter = "PNG Files\0*.PNG\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "png";
    
    if (GetSaveFileNameA(&ofn)) {
        if (image_viewer_->saveImage(filename)) {
            strcpy(status_text_, "Image saved successfully");
        } else {
            strcpy(status_text_, "Failed to save image");
        }
    }
#endif
}

void GUIApp::onLoadModel() {
#if defined(_WIN32)
    OPENFILENAMEA ofn;
    char filename[MAX_PATH] = "";
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = glfwGetWin32Window(window_);
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrFilter = "Model Files\0*.SAFETENSORS;*.CKPT;*.GGUF\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileNameA(&ofn)) {
        strcpy(model_path_, filename);
        strcpy(status_text_, "Loading model...");
        
        if (generator_->loadModel(model_path_, n_threads_)) {
            model_loaded_ = true;
            strcpy(status_text_, "Model loaded successfully");
        } else {
            model_loaded_ = false;
            strcpy(status_text_, "Failed to load model");
        }
    }
#endif
}
