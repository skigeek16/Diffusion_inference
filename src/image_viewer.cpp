#include "image_viewer.h"
#include <glad/glad.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

ImageViewer::ImageViewer() {}

ImageViewer::~ImageViewer() {
    clear();
}

void ImageViewer::loadImage(const uint8_t* data, int width, int height, int channels) {
    printf("ImageViewer::loadImage called: %dx%d, channels=%d\n", width, height, channels);
    
    if (texture_id_) {
        printf("Deleting old texture: %u\n", texture_id_);
        glDeleteTextures(1, &texture_id_);
        texture_id_ = 0;
    }
    
    width_ = width;
    height_ = height;
    image_data_.assign(data, data + (width * height * channels));
    
    glGenTextures(1, &texture_id_);
    printf("Generated texture ID: %u\n", texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    printf("Texture loaded successfully. hasImage: %d\n", hasImage());
}

void ImageViewer::clear() {
    if (texture_id_) {
        glDeleteTextures(1, &texture_id_);
        texture_id_ = 0;
    }
    width_ = 0;
    height_ = 0;
    image_data_.clear();
}

bool ImageViewer::saveImage(const char* filename) {
    if (!hasImage()) return false;
    
    int channels = image_data_.size() / (width_ * height_);
    return stbi_write_png(filename, width_, height_, channels, 
                          image_data_.data(), width_ * channels) != 0;
}
