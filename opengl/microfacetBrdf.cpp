#include "microfacetBrdf.h"

#include <math>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../3rd-party-lib/stb/stb_image_write.h"

#include "../3rd-party-lib/stb/stb_image.h"


namespace CG {

unsigned int MicrofacetBRDF::kEavgTexture = 0;
unsigned int MicrofacetBRDF::kMicroModelBrdfTexture = 0;
std::vector<float> MicrofacetBRDF::kMicroModelBrdfTextureData;
std::vector<float> MicrofacetBRDF::kEavgTextureData;

#define SAMPLE_COUNT 1000
#define PI 3.14159
MicrofacetBRDF::MicrofacetBRDF(glm::vec3 fresnel, float roughness)
    : fresnel_(fresnel)
    , roughness_(roughness) {

}

glm::vec3 MicrofacetBRDF::getBrdf(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal) {

    glm::vec3 Eavg = readValueFromEavgTexture();

    float ViewAngle = glm::dot(glm::normalize(view), glm::normalize(normal));
    float LightAngle = glm::dot(glm::normalize(light), glm::normalize(normal));

    float kullaContyEnergy = (1.0 - readValueFromMicroModelBrdfTexture(ViewAngle)) * (1.0 - readValueFromMicroModelBrdfTexture(LightAngle))/(PI*(1.0 - Eavg));
    glm::vec3 bounceLoss = fresnelAverage_*Eavg/(1.0 - fresnelAverage_*(1.0 - Eavg));

    glm::vec3 MicrofacetBrdf = readValueFromMicroModelBrdfTexture(ViewAngle, fresnel_);

    glm::vec3 brdf = MicrofacetBrdf + bounceLoss * kullaContyEnergy;

    return brdf;
}

bool MicrofacetBRDF::preComputer() {
    MicrofacetBRDF::preComputerMicroModelBrdf();

    MicrofacetBRDF::preComputerEavg();

    MicrofacetBRDF::generateTexture();
}

unsigned int MicrofacetBRDF::preComputerEavg() {
    MicrofacetBRDF::kEavgTextureData.reserve(100*100*3);
    for (float roughness = 0.01; roughness < 1.0; roughness += 0.01) {
        float Eavg = 0.0;
        for (float sinTheta = 0.0; sinTheta < 1.0; sinTheta+=0.01) {
            Eavg = Eavg + 2.0*0.01*sinTheta*MicrofacetBRDF::readValueFromMicroModelBrdfTexture(sinTheta, roughness);
        }

        for (float sinTheta = 0.0; sinTheta < 1.0; sinTheta+=0.01) {
            MicrofacetBRDF::kEavgTextureData[100*3*10000*roughness + 10000*sinTheta] = Eavg;
            MicrofacetBRDF::kEavgTextureData[100*3*10000*roughness + 10000*sinTheta + 1] = Eavg;
            MicrofacetBRDF::kEavgTextureData[100*3*10000*roughness + 10000*sinTheta + 2] = Eavg;
        }
    }
}

unsigned int MicrofacetBRDF::preComputerMicroModelBrdf() {
    MicrofacetBRDF::kMicroModelBrdfTextureData.reserve(100*100*3);
    for (float sinTheta = 0.0; sinTheta < 1.0; sinTheta+=0.01) {
        glm::vec3 viewDirection = glm::vec3(sinTheta, 0.0, sqrtf(1.0 - sinTheta*sinTheta));
        for (float roughness = 0.01; roughness < 1.0; roughness += 0.01) {
            float brdf1 = 0, brdf2 = 0;
            for (unsigned int sample = 0; sample < SAMPLE_COUNT; sample++) {
                glm::vec3 lightDirection = MicrofacetBRDF::ImportanceSampleFromGGX(roughness, viewDirection);

                glm::vec3 half = glm::normalize(glm::normalize(viewDirection) + glm::normalize(lightDirection));
                float cosThetaGeneralNormal = glm::dot(glm::normalize(lightDirection), glm::normalize(0.0, 0.0, 1.0));
                float cosThetaMicrofacetNormal = glm::dot(glm::normalize(lightDirection), half);

                if (glm::vec3(half, lightDirection) > 0.0) {
                    brdf1 = brdf1 + cosThetaGeneralNormal*(1.0 - powf((1 - cosThetaMicrofacetNormal), 5)) * MicrofacetBRDF::MicrofacetModelWithoutFresnel(lightDirection, viewDirection, roughness, half);
                    brdf2 = brdf2 + cosThetaGeneralNormal*powf((1 - cosThetaMicrofacetNormal), 5) * MicrofacetBRDF::MicrofacetModelWithoutFresnel(lightDirection, viewDirection, roughness, half); 
                }
          
            }
            brdf1 = brdf1/SAMPLE_COUNT;
            brdf2 = brdf2/SAMPLE_COUNT;
            MicrofacetBRDF::kMicroModelBrdfTextureData[100*3*10000*roughness + 10000*sinTheta] = brdf1;
            MicrofacetBRDF::kMicroModelBrdfTextureData[100*3*10000*roughness + 10000*sinTheta + 1] = brdf2;
            MicrofacetBRDF::kMicroModelBrdfTextureData[100*3*10000*roughness + 10000*sinTheta + 2] = 0.0;
        }
    }
}

float MicrofacetBRDF::MicrofacetModelWithoutFresnel(glm::vec3 lightDirection, glm::vec3 viewDirection, float roughness, glm::vec3 half) {
    float G = MicrofacetBRDF::shadowMasking(lightDirection, viewDirection, roughness, half);
    float result = glm::dot(viewDirection, half)*G/(glm::dot(viewDirection, glm::vec3(0.0, 0.0, 1.0))*glm::dot(half, glm::vec3(0.0, 0.0, 1.0)))
    return result;
}

glm::vec3 MicrofacetBRDF::fresnel(glm::vec3 lightDirection, glm::vec3 viewDirection, glm::vec3 half, glm::vec3 R) {
    return (R + (glm::vec3(1.0, 1.0, 1.0) - R)*powf((1.0 - glm::dot(lightDirection, half)), 5));
}

float MicrofacetBRDF::normalDistributionFunction(glm::vec3 lightDirection, glm::vec3 viewDirection, float roughness, glm::vec3 half) {
    float alpha = roughness * roughness;
    float result1 = powf(glm::dot(half, glm::vec3(0.0, 0.0, 1.0)), 2)*(alpha*alpha - 1.0) + 1.0;
    float result = (alpha * alpha)/(PI*powf(result1, 2));
    return result;
}

float MicrofacetBRDF::shadowMasking(glm::vec3 lightDirection, glm::vec3 viewDirection, float roughness, glm::vec3 half) {
    float k = powf((roughness + 1), 2)/8.0;
    float result1 = glm::dot(half, lightDirection)/(glm::dot(half, lightDirection)*(1.0 - k) + k);
    float result2 = glm::dot(half, viewDirection)/(glm::dot(half, viewDirection)*(1.0 - k) + k);
    return (result1*result2);
}

glm::vec3 MicrofacetBRDF::ImportanceSampleFromGGX(const float roughness, const glm::vec3 viewDirection) {
    float rand1 = (float)(rand()%1000)/1000.0;
    float rand2 = (float)(rand()%1000)/1000.0;
    float theta = atan(roughness*sqrtf(rand1)/sqrtf(1 - rand1));
    float fi = 2.0*PI*rand2;
    glm::vec3 normal = glm::vec3(cos(theta)*cos(fi), cos(theta)*sin(fi), sin(theta));
    glm::vec3 lightDirection = 2.0*glm::dot(normal, viewDirection)*normal - viewDirection;
    return lightDirection;
}

float MicrofacetBRDF::getPDFByNormalFromGGX(glm::vec3 normal) {

}

void MicrofacetBRDF::preComputerFresnelAverage() {
    fresnelAverage_ = glm::vec3(0.0, 0.0, 0.0);
    for (float sinTheta = 0.0; sinTheta < 1.0; sinTheta+=0.01) {
        float cosTheta = sqrtf(1.0 - sinTheta*sinTheta);
        glm::vec3 f = fresnel_ + (glm::vec(1.0, 1.0, 1.0) - fresnel_)*pow((1.0 - cosTheta), 5);
        f = 2.0 * f * sinTheta * 0.01;
        fresnelAverage_ = fresnelAverage_ + f;
    }
}

glm::vec3 MicrofacetBRDF::readValueFromEavgTexture(const float roughness) {
    float Eavg = MicrofacetBRDF::kEavgTextureData[100*3*10000*roughness];
    return glm::vec3(Eavg, Eavg, Eavg);
}

glm::vec3 MicrofacetBRDF::readValueFromMicroModelBrdfTexture(const float sinTheta, const float roughness, const glm::vec3 fresnel) {
    float brdf1 = MicrofacetBRDF::kMicroModelBrdfTextureData[100*3*10000*roughness + 10000*sinTheta];
    float brdf2 = MicrofacetBRDF::kMicroModelBrdfTextureData[100*3*10000*roughness + 10000*sinTheta + 1];
    return (brdf1*fresnel + brdf2*glm::vec3(1.0, 1.0, 1.0));
}

void MicrofacetBRDF::generateTexture() {
    std::vector<uint8_t> MicroModelBrdfImageData;
    for (auto& it : MicrofacetBRDF::kMicroModelBrdfTextureData) {
        MicroModelBrdfImageData.push_back(uint8_t(it*256.0));
    }
    stbi_write_png("MicroModelBrdfImage.png", 100, 100, 3, MicroModelBrdfImageData.data(), 3*sizeof(uint8_t));
    MicrofacetBRDF::kMicroModelBrdfTexture = MicrofacetBRDF::loadTexture("MicroModelBrdfImage.png");

    std::vector<uint8_t> EavgImageData;
    for (auto& it : MicrofacetBRDF::kEavgTextureData) {
        EavgImageData.push_back(uint8_t(it*256.0));
    }
    stbi_write_png("EavgImage.png", 100, 100, 3, EavgImageData.data(), 3*sizeof(uint8_t));
    MicrofacetBRDF::kEavgTexture = MicrofacetBRDF::loadTexture("EavgImage.png");
}

unsigned int MicrofacetBRDF::loadTexture(std::string file) {
    int textureWidth, textureHeight, nrComponents;
    unsigned char* data = stbi_load(file.c_str(), &textureWidth, &textureHeight, &nrComponents, 0);
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

        float fLargest;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
        glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);

        return textureId;
    } else {
        exit(1);
    }
}

}