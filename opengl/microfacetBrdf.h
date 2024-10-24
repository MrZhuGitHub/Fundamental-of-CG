#ifndef _MICROFACET_BRDF_H_
#define _MICROFACET_BRDF_H_

#include "BRDF.h"

#include <memory>
#include <vector>
#include <string>

namespace CG {

class MicrofacetBRDF : public BRDF {
public:
    MicrofacetBRDF(glm::vec3 fresnel, float roughness);
    glm::vec3 getBrdf(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal) override;
    void preComputerFresnelAverage();
    static bool preComputer();
    static void getPreComputerResult(unsigned int& EavgTexture, unsigned int& MicroModelBrdfTexture) {
        EavgTexture = kEavgTexture;
        MicroModelBrdfTexture = kMicroModelBrdfTexture;
    }

private:
    static unsigned int preComputerEavg();
    static unsigned int preComputerMicroModelBrdf();
    static float MicrofacetModelWithoutFresnel(glm::vec3 lightDirection, glm::vec3 viewDirection, float roughness, glm::vec3 half);
    static glm::vec3 fresnel(glm::vec3 lightDirection, glm::vec3 viewDirection, glm::vec3 half, glm::vec3 R);
    static float normalDistributionFunction(glm::vec3 lightDirection, glm::vec3 viewDirection, float roughness, glm::vec3 half); //GGX Model
    static float shadowMasking(glm::vec3 lightDirection, glm::vec3 viewDirection, float roughness, glm::vec3 half); //Smith Model
    static glm::vec3 ImportanceSampleFromGGX(const float roughness, const glm::vec3 viewDirection); //sample normal
    static float getPDFByNormalFromGGX(glm::vec3 normal);

    static glm::vec3 readValueFromEavgTexture(const float roughness);
    static glm::vec3 readValueFromMicroModelBrdfTexture(const float sinTheta, const float roughness, const glm::vec3 fresnel = glm::vec3(1.0, 1.0, 1.0));

    static void generateTexture();

    static unsigned int loadTexture(const char* file);

private:
    glm::vec3 fresnel_;
    float roughness_;
    glm::vec3 fresnelAverage_;
    static unsigned int kEavgTexture;
    static unsigned int kMicroModelBrdfTexture;
    static std::vector<float> kMicroModelBrdfTextureData;
    static std::vector<float> kEavgTextureData;
};

}

#endif