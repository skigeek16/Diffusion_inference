/*
 * Example using real Stable Diffusion models
 * via stable-diffusion.cpp library
 */

#include "sd_wrapper.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "=== Real Stable Diffusion Inference ===" << std::endl;
    
    // Default model path (adjust to your model location)
    std::string model_path = "models/sd-v1-5-pruned-emaonly.safetensors";
    
    if (argc > 1) {
        model_path = argv[1];
    }
    
    try {
        // Initialize Stable Diffusion
        sd_real::StableDiffusion sd;
        
        // Configure model
        sd_real::ModelConfig config;
        config.model_path = model_path;
        config.n_threads = 8;
        config.flash_attention = true;
        
        std::cout << "\nLoading model: " << config.model_path << std::endl;
        if (!sd.load_model(config)) {
            std::cerr << "Failed to load model!" << std::endl;
            std::cerr << "\nMake sure you have downloaded a model file." << std::endl;
            std::cerr << "See REAL_MODELS.md for instructions." << std::endl;
            return 1;
        }
        
        // Example 1: Simple text-to-image
        {
            std::cout << "\n--- Example 1: Basic Text-to-Image ---" << std::endl;
            
            sd_real::GenerationParams params;
            params.prompt = "a beautiful sunset over mountains, highly detailed, 4k";
            params.negative_prompt = "blurry, low quality, distorted";
            params.width = 512;
            params.height = 512;
            params.steps = 25;
            params.cfg_scale = 7.5f;
            params.sample_method = "euler_a";
            params.scheduler = "karras";
            params.seed = 42;
            
            auto image = sd.generate(params);
            image->save_png("output_sunset.png");
        }
        
        // Example 2: Different prompt and settings
        {
            std::cout << "\n--- Example 2: Portrait ---" << std::endl;
            
            sd_real::GenerationParams params;
            params.prompt = "portrait of a young woman, natural lighting, detailed face, professional photography";
            params.negative_prompt = "cartoon, anime, low quality, blurry";
            params.width = 512;
            params.height = 768;  // Portrait aspect ratio
            params.steps = 30;
            params.cfg_scale = 8.0f;
            params.sample_method = "dpmpp2m";
            params.scheduler = "karras";
            params.seed = 123;
            
            auto image = sd.generate(params);
            image->save_png("output_portrait.png");
        }
        
        // Example 3: Landscape with different sampler
        {
            std::cout << "\n--- Example 3: Landscape ---" << std::endl;
            
            sd_real::GenerationParams params;
            params.prompt = "epic fantasy landscape, castle on a mountain, dramatic clouds, cinematic";
            params.negative_prompt = "blurry, bad quality";
            params.width = 768;
            params.height = 512;  // Landscape aspect ratio
            params.steps = 28;
            params.cfg_scale = 7.0f;
            params.sample_method = "heun";
            params.seed = 999;
            
            auto image = sd.generate(params);
            image->save_png("output_landscape.png");
        }
        
        // Example 4: Random seed generation
        {
            std::cout << "\n--- Example 4: Random Seed ---" << std::endl;
            
            sd_real::GenerationParams params;
            params.prompt = "cute robot toy, studio lighting, product photography";
            params.negative_prompt = "blurry";
            params.width = 512;
            params.height = 512;
            params.steps = 20;
            params.cfg_scale = 7.5f;
            params.seed = -1;  // Random seed
            
            auto image = sd.generate(params);
            image->save_jpg("output_robot.jpg", 95);
        }
        
        std::cout << "\n=== All generations complete! ===" << std::endl;
        std::cout << "Check the output PNG files in the current directory." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
