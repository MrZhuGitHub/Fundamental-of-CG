#ifndef _MICROFACET_BRDF_H_
#define _MICROFACET_BRDF_H_

#include "BRDF.h"

#include <memory>

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
    static float MicrofacetModelWithoutFresnel(glm::vec3 inDirection, glm::vec3 outDirection, float roughness, glm::vec3 normal = glm::vec3(0.0, 0.0, 1.0));
    static float fresnel(glm::vec3 inDirection, glm::vec3 outDirection, float roughness, glm::vec3 normal = glm::vec3(0.0, 0.0, 1.0));
    static float normalDistributionFunction(glm::vec3 inDirection, glm::vec3 outDirection, float roughness, glm::vec3 normal = glm::vec3(0.0, 0.0, 1.0)); //GGX Model
    static float shadowMasking(glm::vec3 inDirection, glm::vec3 outDirection, float roughness, glm::vec3 normal = glm::vec3(0.0, 0.0, 1.0)); //Smith Model
    static glm::vec3 ImportanceSampleFromGGX(); //sample normal
    static float getPDFByNormalFromGGX(glm::vec3 normal);

    glm::vec3 getValueFromEavgTexture();
    glm::vec3 getValueFromMicroModelBrdfTexture(const float angle, const glm::vec3 fresnel = glm::vec3(1.0, 1.0, 1.0));

private:
    glm::vec3 fresnel_;
    float roughness_;
    glm::vec3 fresnelAverage_;
    static unsigned int kEavgTexture;
    static unsigned int kMicroModelBrdfTexture;
}

}

#endif