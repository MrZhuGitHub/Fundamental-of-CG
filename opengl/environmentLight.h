#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "framebuffer.h"

#include "shader.h"

#include <map>
#include <memory>
#include <thread>
#include <chrono>

enum CubeTextureId {
    CUBE_TEXTURE_DOWN = 0,
    CUBE_TEXTURE_UP,
    CUBE_TEXTURE_FRONT,
    CUBE_TEXTURE_BACK,
    CUBE_TEXTURE_LEFT,
    CUBE_TEXTURE_RIGHT,
    CUBE_TEXTURE_INVALID,
};

using imageFilePath = std::string;
using cubemapDirection = unsigned int;

class GLFWwindow;

namespace CG {

class EnvironmentLight{
public:
    EnvironmentLight(std::map<CubeTextureId, imageFilePath> IBLs, unsigned int width, unsigned int height);

    glm::vec3 getRadiance(glm::vec3 direction, float roughness);

    bool preComputerEnvironmentLight(GLFWwindow* window);

    unsigned int getPreComputerResult();

private:

    unsigned int gernerateCubeTexture();

    unsigned int gerneratePrefilterMap();

    void initCubeRender();

public:
    
    std::map<CubeTextureId, imageFilePath> IBLs_;
    unsigned int IblMipmapTexture_;
    unsigned int cubeTexture_;
    unsigned int iblResolutionWidth_;
    unsigned int iblResolutionHeight_;
    using viewMatrix = glm::mat4;
    std::map<CubeTextureId, viewMatrix> cameraViewMats_;
    std::map<CubeTextureId, cubemapDirection> cubemapDirections_;
    glm::mat4 projectMatrix_;
    unsigned int IblFrameBufferId_;
    unsigned int IblRenderBufferId_;
    std::shared_ptr<shader> cubeRenderShader_;
    unsigned int VAO_;
    unsigned int VBO_;
};

}

#endif