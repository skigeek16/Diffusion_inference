#pragma once

#include <vector>
#include <cstdint>

class ImageViewer {
public:
    ImageViewer();
    ~ImageViewer();

    void loadImage(const uint8_t* data, int width, int height, int channels);
    void clear();
    
    unsigned int getTextureID() const { return texture_id_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    bool hasImage() const { return texture_id_ != 0; }
    
    bool saveImage(const char* filename);

private:
    unsigned int texture_id_ = 0;
    int width_ = 0;
    int height_ = 0;
    std::vector<uint8_t> image_data_;
};
