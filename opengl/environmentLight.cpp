#include "environmentLight.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

// #define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../3rd-party-lib/stb/stb_image_write.h"

#include "../3rd-party-lib/stb/stb_image.h"

#define MAX_MIPMAP_LEVEL 6

namespace CG {

EnvironmentLight::EnvironmentLight(std::map<CubeTextureId, imageFilePath> IBLs, unsigned int width, unsigned int height)
    : IBLs_(IBLs)
    , iblResolutionWidth_(width)
    , iblResolutionHeight_(height) {

    cubeTexture_ = gernerateCubeTexture();
    IblMipmapTexture_ = gerneratePrefilterMap();

    cubeRenderShader_ = std::make_shared<shader>("/opengles/Fundamental-of-CG/opengl/shader/PrefilterIBLVertex.glsl",
                                                 "/opengles/Fundamental-of-CG/opengl/shader/PrefilterIBLFragment.glsl");

    cameraViewMats_.insert(std::make_pair(CUBE_TEXTURE_DOWN,  glm::lookAt(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 1.0, 0.0))));
    cameraViewMats_.insert(std::make_pair(CUBE_TEXTURE_UP,    glm::lookAt(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0))));
    cameraViewMats_.insert(std::make_pair(CUBE_TEXTURE_FRONT, glm::lookAt(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0))));
    cameraViewMats_.insert(std::make_pair(CUBE_TEXTURE_BACK,  glm::lookAt(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0))));
    cameraViewMats_.insert(std::make_pair(CUBE_TEXTURE_LEFT,  glm::lookAt(glm::vec3(0.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))));
    cameraViewMats_.insert(std::make_pair(CUBE_TEXTURE_RIGHT, glm::lookAt(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0))));
    projectMatrix_ = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1.0f);

    cubemapDirections_.insert(std::make_pair(CUBE_TEXTURE_DOWN,  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y));
    cubemapDirections_.insert(std::make_pair(CUBE_TEXTURE_UP,    GL_TEXTURE_CUBE_MAP_POSITIVE_Y));
    cubemapDirections_.insert(std::make_pair(CUBE_TEXTURE_FRONT, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z));
    cubemapDirections_.insert(std::make_pair(CUBE_TEXTURE_BACK,  GL_TEXTURE_CUBE_MAP_POSITIVE_Z));
    cubemapDirections_.insert(std::make_pair(CUBE_TEXTURE_LEFT,  GL_TEXTURE_CUBE_MAP_NEGATIVE_X));
    cubemapDirections_.insert(std::make_pair(CUBE_TEXTURE_RIGHT, GL_TEXTURE_CUBE_MAP_POSITIVE_X));
}

glm::vec3 EnvironmentLight::getRadiance(glm::vec3 direction, float roughness) {

}

bool EnvironmentLight::preComputerEnvironmentLight(GLFWwindow* window) {

    glGenFramebuffers(1, &IblFrameBufferId_);
    glGenRenderbuffers(1, &IblRenderBufferId_);
    
    for (int mipmapLevel = 0; mipmapLevel < MAX_MIPMAP_LEVEL; mipmapLevel++) {
        unsigned int resolutionWidth = iblResolutionWidth_*powf(0.5, mipmapLevel);
        unsigned int resolutionHeight = iblResolutionHeight_*powf(0.5, mipmapLevel);

        for (auto& view : cameraViewMats_) {

            glBindFramebuffer(GL_FRAMEBUFFER, IblFrameBufferId_);

            glBindRenderbuffer(GL_RENDERBUFFER, IblRenderBufferId_);

            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolutionWidth, resolutionHeight);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cubemapDirections_.at(view.first), IblMipmapTexture_, mipmapLevel);

            cubeRenderShader_->use();

            glViewport(0, 0, resolutionWidth, resolutionHeight);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float roughness = (float)mipmapLevel/(MAX_MIPMAP_LEVEL - 1.0);

            cubeRenderShader_->setFloat("roughness", roughness);
            cubeRenderShader_->setFloat("resolutionOfEnvironmentMap", (iblResolutionWidth_ > iblResolutionHeight_ ? iblResolutionWidth_ : iblResolutionHeight_));

            cubeRenderShader_->setViewMatrix(view.second);
            cubeRenderShader_->setProjectionMatrix(projectMatrix_);
            cubeRenderShader_->setModelMatrix(glm::mat4(1.0f));

            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture_);

            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

            cubeRender();

            //todo: render to screen

            glBindFramebuffer(GL_READ_FRAMEBUFFER, IblFrameBufferId_);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, resolutionWidth, resolutionHeight, 0, 0, resolutionWidth, resolutionHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

            //swap frame buffer
            glfwSwapBuffers(window);
            glfwPollEvents();

            std::cout << "complete one render" << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

unsigned int EnvironmentLight::getPreComputerResult() {
    return IblMipmapTexture_;
}

unsigned int EnvironmentLight::gernerateCubeTexture() {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannnels;
    for (auto& image : IBLs_) {
        unsigned char* data = stbi_load(image.second.c_str(), &width, &height, &nrChannnels, 0);
        if (data) {
            glTexImage2D(image.first, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data); 
        } else {
            std::cout << "Cubemap texture failed to load at path: " << image.second.c_str() << std::endl;
            stbi_image_free(data); 
            exit(0); 
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return textureID;
}

void EnvironmentLight::cubeRender() {

        float cube[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };

        unsigned int VBO;
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0 + 3 * sizeof(float));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0 + 6 * sizeof(float));
        glEnableVertexAttribArray(2);

        glDrawArrays(GL_TRIANGLES, 0, 36);
}

unsigned int EnvironmentLight::gerneratePrefilterMap() {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (auto& image : IBLs_) {
        glTexImage2D(image.first, 0, GL_RGB, iblResolutionWidth_, iblResolutionHeight_, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return textureID;
}

}