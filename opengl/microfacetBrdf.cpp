#include "microfacetBrdf.h"

#include <math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../3rd-party-lib/stb/stb_image_write.h"

#include "../3rd-party-lib/stb/stb_image.h"

// #include <GL/glew.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>


namespace CG {

unsigned int MicrofacetBRDF::kEavgTexture = 0;
unsigned int MicrofacetBRDF::kMicroModelBrdfTexture = 0;
std::vector<float> MicrofacetBRDF::kMicroModelBrdfTextureData;
std::vector<float> MicrofacetBRDF::kEavgTextureData;

#define SAMPLE_COUNT 100000
#define PI 3.14159
MicrofacetBRDF::MicrofacetBRDF(glm::vec3 fresnel, float roughness)
    : fresnel_(fresnel)
    , roughness_(roughness) {

}

glm::vec3 MicrofacetBRDF::getBrdf(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal) {

    // glm::vec3 Eavg = MicrofacetBRDF::readValueFromEavgTexture();

    // float ViewAngle = glm::dot(glm::normalize(view), glm::normalize(normal));
    // float LightAngle = glm::dot(glm::normalize(light), glm::normalize(normal));

    // float kullaContyEnergy = (1.0 - MicrofacetBRDF::readValueFromMicroModelBrdfTexture(ViewAngle)) * (1.0 - MicrofacetBRDF::readValueFromMicroModelBrdfTexture(LightAngle))/(PI*(1.0 - Eavg));
    // glm::vec3 bounceLoss = fresnelAverage_*Eavg/(1.0 - fresnelAverage_*(1.0 - Eavg));

    // glm::vec3 MicrofacetBrdf = MicrofacetBRDF::readValueFromMicroModelBrdfTexture(ViewAngle, fresnel_);

    // glm::vec3 brdf = MicrofacetBrdf + bounceLoss * kullaContyEnergy;

    // return brdf;
}

bool MicrofacetBRDF::preComputer() {
    MicrofacetBRDF::preComputerMicroModelBrdf();

    MicrofacetBRDF::preComputerEavg();

    MicrofacetBRDF::generateTexture();

    return true;
}

unsigned int MicrofacetBRDF::preComputerEavg() {
    for (unsigned int i = 1; i <= 100; i++) {
        glm::vec3 Eavg = glm::vec3(0.0, 0.0, 0.0);
        for (unsigned int j = 1; j <= 100; j++) {
            float sinTheta = (float)(2*(j - 1))/200.0;
            float cosTheta = sqrtf(1.0 - sinTheta*sinTheta);
            Eavg = Eavg + glm::vec3(2.0, 2.0, 2.0)*glm::vec3(0.01, 0.01, 0.01)*glm::vec3(sinTheta, sinTheta, sinTheta)*MicrofacetBRDF::readValueFromMicroModelBrdfTexture(j, i);
        }
        for (unsigned int j = 1; j <= 100; j++) {
            MicrofacetBRDF::kEavgTextureData.push_back(Eavg[0]);
            MicrofacetBRDF::kEavgTextureData.push_back(Eavg[1]);
            MicrofacetBRDF::kEavgTextureData.push_back(Eavg[2]);
        }
    }
}

unsigned int MicrofacetBRDF::preComputerMicroModelBrdf() {
    for (unsigned int i = 1; i <= 100; i++) {
        float cosTheta = i*0.01;
        float sinTheta = sqrtf(1.0 - cosTheta*cosTheta);
        glm::vec3 viewDirection = glm::vec3(sinTheta, 0.0, cosTheta);
        for (unsigned int j = 1; j <= 100; j++) {
            float roughness = j*0.01;
            float brdf1 = 0, brdf2 = 0;
            for (unsigned int sample = 0; sample < SAMPLE_COUNT; sample++) {
                glm::vec3 lightDirection = MicrofacetBRDF::ImportanceSampleFromGGX(roughness, viewDirection);
                if (lightDirection.length() > 0.9) {
                    glm::vec3 half = glm::normalize(glm::normalize(viewDirection) + glm::normalize(lightDirection));
                    float cosThetaGeneralNormal = glm::dot(glm::normalize(lightDirection), glm::vec3(0.0, 0.0, 1.0));
                    float cosThetaMicrofacetNormal = glm::dot(glm::normalize(lightDirection), half);

                    if (cosThetaGeneralNormal > 0.0) {
                        brdf1 = brdf1 + (1.0 - powf((1 - cosThetaMicrofacetNormal), 5)) * MicrofacetBRDF::MicrofacetModelWithoutFresnel(lightDirection, viewDirection, roughness, half);
                        brdf2 = brdf2 + powf((1 - cosThetaMicrofacetNormal), 5) * MicrofacetBRDF::MicrofacetModelWithoutFresnel(lightDirection, viewDirection, roughness, half); 
                    }
                }
            }
            brdf1 = brdf1/SAMPLE_COUNT;
            brdf2 = brdf2/SAMPLE_COUNT;

            MicrofacetBRDF::kMicroModelBrdfTextureData.push_back(brdf1);
            MicrofacetBRDF::kMicroModelBrdfTextureData.push_back(brdf2);
            MicrofacetBRDF::kMicroModelBrdfTextureData.push_back(0.0);
        }
    }
}

float MicrofacetBRDF::MicrofacetModelWithoutFresnel(glm::vec3 lightDirection, glm::vec3 viewDirection, float roughness, glm::vec3 half) {
    float G = MicrofacetBRDF::shadowMasking(lightDirection, viewDirection, roughness, half);
    float result = glm::dot(viewDirection, half)*G/(glm::dot(viewDirection, glm::vec3(0.0, 0.0, 1.0))*glm::dot(half, glm::vec3(0.0, 0.0, 1.0)));
    return result;
}

glm::vec3 MicrofacetBRDF::fresnel(glm::vec3 lightDirection, glm::vec3 viewDirection, glm::vec3 half, glm::vec3 R) {
    float result = powf((1.0 - glm::dot(lightDirection, half)), 5);
    return (R + (glm::vec3(1.0, 1.0, 1.0) - R)*glm::vec3(result, result, result));
}

float MicrofacetBRDF::normalDistributionFunction(glm::vec3 lightDirection, glm::vec3 viewDirection, float roughness, glm::vec3 half) {
    float alpha = roughness * roughness;
    float result1 = powf(glm::dot(half, glm::vec3(0.0, 0.0, 1.0)), 2)*(alpha*alpha - 1.0) + 1.0;
    float result = (alpha * alpha)/(PI*powf(result1, 2));
    return result;
}

float MicrofacetBRDF::shadowMasking(glm::vec3 lightDirection, glm::vec3 viewDirection, float roughness, glm::vec3 half) {
    float k = roughness*roughness/2.0;
    float result1 = glm::dot(glm::vec3(0.0, 0.0, 1.0), lightDirection)/(glm::dot(glm::vec3(0.0, 0.0, 1.0), lightDirection)*(1.0 - k) + k);
    float result2 = glm::dot(glm::vec3(0.0, 0.0, 1.0), viewDirection)/(glm::dot(glm::vec3(0.0, 0.0, 1.0), viewDirection)*(1.0 - k) + k);
    return (result1*result2);
}

glm::vec3 MicrofacetBRDF::ImportanceSampleFromGGX(const float roughness, const glm::vec3 viewDirection) {
    float rand1 = (float)(rand()%1000)/1000.0;
    float rand2 = (float)(rand()%1000)/1000.0;
    float theta = acos(sqrtf((1 - rand1)/(1 + (powf(roughness, 4) - 1)*rand1)));
    float fi = 2.0*PI*rand2;
    glm::vec3 normal = glm::vec3(sin(theta)*cos(fi), sin(theta)*sin(fi), cos(theta));
    if (glm::dot(normal, viewDirection) > 0) {
        
        glm::vec3 lightDirection = normal*2.0f*glm::dot(normal, viewDirection) - viewDirection;
        lightDirection = glm::normalize(lightDirection);
        return lightDirection;
    } else {
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

float MicrofacetBRDF::getPDFByNormalFromGGX(glm::vec3 normal) {

}

void MicrofacetBRDF::preComputerFresnelAverage() {
    fresnelAverage_ = glm::vec3(0.0, 0.0, 0.0);
    for (float sinTheta = 0.0; sinTheta < 1.0; sinTheta+=0.01) {
        float cosTheta = sqrtf(1.0 - sinTheta*sinTheta);
        glm::vec3 f = fresnel_ + (glm::vec3(1.0, 1.0, 1.0) - fresnel_)*powf((1.0 - cosTheta), 5);
        f = 2.0f * f * sinTheta * 0.01f;
        fresnelAverage_ = fresnelAverage_ + f;
    }
}

glm::vec3 MicrofacetBRDF::readValueFromEavgTexture(const float roughness) {
    float Eavg = MicrofacetBRDF::kEavgTextureData[100*3*100*(roughness - 0.01)];
    return glm::vec3(Eavg, Eavg, Eavg);
}

glm::vec3 MicrofacetBRDF::readValueFromMicroModelBrdfTexture(const unsigned int cosTheta, const unsigned int roughness, const glm::vec3 fresnel) {
    float brdf1 = MicrofacetBRDF::kMicroModelBrdfTextureData[100*3*(cosTheta - 1) + 3*(roughness - 1)];
    float brdf2 = MicrofacetBRDF::kMicroModelBrdfTextureData[100*3*(cosTheta - 1) + 3*(roughness - 1) + 1];
    return (brdf1*fresnel + brdf2*glm::vec3(1.0, 1.0, 1.0));
}

void MicrofacetBRDF::generateTexture() {
    std::vector<uint8_t> MicroModelBrdfImageData;
    for (auto& it : MicrofacetBRDF::kMicroModelBrdfTextureData) {
        MicroModelBrdfImageData.push_back(uint8_t(it*255.0));
    }
    stbi_write_png("MicroModelBrdfImage.png", 100, 100, 3, MicroModelBrdfImageData.data(), 0);
    //MicrofacetBRDF::kMicroModelBrdfTexture = MicrofacetBRDF::loadTexture("/opengles/Fundamental-of-CG/opengl/build/MicroModelBrdfImage.png");

    std::vector<uint8_t> EavgImageData;
    for (auto& it : MicrofacetBRDF::kEavgTextureData) {
        EavgImageData.push_back(uint8_t(it*255.0));
    }
    stbi_write_png("EavgImage.png", 100, 100, 3, EavgImageData.data(), 0);
    //MicrofacetBRDF::kEavgTexture = MicrofacetBRDF::loadTexture("/opengles/Fundamental-of-CG/opengl/build/EavgImage.png");
}

unsigned int MicrofacetBRDF::loadTexture(const char* file) {
    int textureWidth, textureHeight, nrComponents;
    unsigned char* data = stbi_load(file, &textureWidth, &textureHeight, &nrComponents, 0);
    unsigned int textureId;
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            exit(1);
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        // float fLargest;
        // glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
        // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
        glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);

        return textureId;
    } else {
        std::cout << "png image load failed :" << file << std::endl;
        exit(1);
    }
}

}