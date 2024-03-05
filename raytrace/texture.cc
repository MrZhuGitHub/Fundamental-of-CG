#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "../3rd-party-lib/stb/stb_image.h"

namespace CG {

bool texture::loadTexture(std::string filePath) {
    unsigned char* data = nullptr;
    int imageWidth, imageHeight;
    const int bytesPerPixel = 3;
    int actualBytesPerPixel;
    data = stbi_load(filePath.c_str(), &imageWidth, &imageHeight, &actualBytesPerPixel, bytesPerPixel);
    if (nullptr == data) {
        return false;
    }
    colorMaps_.clear();
    for (int height = 0; height < imageHeight; height++) {
        for (int width = 0; width < imageWidth; width++) {
            int index = height * imageWidth * actualBytesPerPixel + width * actualBytesPerPixel;
            double r = (double)(data[index])/255.0;
            double g = (double)(data[index + 1])/255.0;
            double b = (double)(data[index + 2])/255.0;
            color pixelColor(r, g, b);
            colorMaps_.insert(std::pair<unsigned int, color>((height * imageWidth + width), pixelColor));
        }
    }
    imageWidth_ = imageWidth;
    imageHeight_ = imageHeight;
    return true;
}

color texture::lookup(float u, float v) {
    int height = static_cast<int>(v * imageHeight_);
    int width = static_cast<int>(u * imageWidth_);
    auto it = colorMaps_.find(height*imageWidth_+width);
    if (it == colorMaps_.end()) {
        return color(1, 1, 1);
    } else {
        return it->second;
    }
}

}