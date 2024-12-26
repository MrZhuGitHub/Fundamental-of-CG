#include "texture.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../3rd-party-lib/stb/stb_image.h"

namespace CG {

textureLoader::textureLoader()
    : textureIfValid_(false) {

}

bool textureLoader::set2Dtexture(std::string image, bool mipmap) {
    glGetError();

    glGenTextures(1, &textureId_);
    glBindTexture(GL_TEXTURE_2D, textureId_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (mipmap) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    int width, height, nrChannels;
    unsigned char* data = stbi_load(image.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        if (mipmap) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    } else {
        stbi_image_free(data);
        textureIfValid_ = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }

    if (0 == glGetError()) {
        stbi_image_free(data);
        textureIfValid_ = true;
        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    } else {
        stbi_image_free(data);
        textureIfValid_ = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }
}

bool textureLoader::set3Dtexture(std::map<CubeTextureId, std::string> images, bool mipmap) {
    return false;
}

unsigned int textureLoader::getTextureId() {
    return textureId_;
}

textureLoader::~textureLoader() {
    if (textureIfValid_) {
        glDeleteTextures(1, &textureId_);
    }
}

}