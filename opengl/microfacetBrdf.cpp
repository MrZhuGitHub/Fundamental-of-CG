#include "microfacetBrdf.h"

namespace CG {

unsigned int MicrofacetBRDF::kEavgTexture = 0;
unsigned int MicrofacetBRDF::kMicroModelBrdfTexture = 0;

#define SAMPLE_COUNT 1000
#define PI 3.14159
MicrofacetBRDF::MicrofacetBRDF(glm::vec3 fresnel, float roughness)
    : fresnel_(fresnel)
    , roughness_(roughness) {

}

glm::vec3 MicrofacetBRDF::getBrdf(const glm::vec3& view, const glm::vec3& light, const glm::vec3& normal) {

    glm::vec3 Eavg = getValueFromEavgTexture();

    float ViewAngle = glm::dot(glm::normalize(view), glm::normalize(normal));
    float LightAngle = glm::dot(glm::normalize(light), glm::normalize(normal));

    float kullaContyEnergy = (1.0 - getValueFromMicroModelBrdfTexture(ViewAngle)) * (1.0 - getValueFromMicroModelBrdfTexture(LightAngle))/(PI*(1.0 - Eavg));
    glm::vec3 bounceLoss = fresnelAverage_*Eavg/(1.0 - fresnelAverage_*(1.0 - Eavg));

    glm::vec3 MicrofacetBrdf = getValueFromMicroModelBrdfTexture(ViewAngle, fresnel_);

    glm::vec3 brdf = MicrofacetBrdf + bounceLoss * kullaContyEnergy;

    return brdf;
}

bool MicrofacetBRDF::preComputer() {

    MicrofacetBRDF::kEavgTexture = MicrofacetBRDF::preComputerEavg();

    MicrofacetBRDF::kMicroModelBrdfTexture = MicrofacetBRDF::preComputerMicroModelBrdf();
}

unsigned int MicrofacetBRDF::preComputerEavg() {
    //Based on preComputerMicroModelBrdf
}

unsigned int MicrofacetBRDF::preComputerMicroModelBrdf() {
    for (float angle = 0.0; angle < 90.0; angle += 1.0) {
        for (float roughness = 0.01; roughness < 1.0; roughness += 0.01) {
            float brdf1 = 0, brdf2 = 0;
            for (unsigned int sample = 0; sample < SAMPLE_COUNT; sample++) {
                glm::vec3 inDirection = MicrofacetBRDF::ImportanceSampleFromGGX();
                float pdf = MicrofacetBRDF::getPDFByNormalFromGGX(inDirection);

                float cosInputAngle = glm::dot(glm::normalize(inDirection), glm::vec3(0.0, 0.0, 1.0))
                brdf1 = brdf1 + cosInputAngle*(1.0 - powf((1 - cos(angle)), 5)) * MicrofacetModelWithoutFresnel() / pdf;
                brdf2 = brdf2 + cosInputAngle*powf((1 - cos(angle)), 5) * MicrofacetModelWithoutFresnel() / pdf;

            }
            brdf1 = brdf1/SAMPLE_COUNT;
            brdf2 = brdf2/SAMPLE_COUNT;
        }
    }
}

float MicrofacetBRDF::MicrofacetModelWithoutFresnel(glm::vec3 inDirection, glm::vec3 outDirection, float roughness, glm::vec3 normal) {

}

float MicrofacetBRDF::fresnel(glm::vec3 inDirection, glm::vec3 outDirection, float roughness, glm::vec3 normal) {

}

float MicrofacetBRDF::normalDistributionFunction(glm::vec3 inDirection, glm::vec3 outDirection, float roughness, glm::vec3 normal) {

}

float MicrofacetBRDF::shadowMasking(glm::vec3 inDirection, glm::vec3 outDirection, float roughness, glm::vec3 normal) {

}

glm::vec3 MicrofacetBRDF::ImportanceSampleFromGGX() {

}

float MicrofacetBRDF::getPDFByNormalFromGGX(glm::vec3 normal) {

}

void MicrofacetBRDF::preComputerFresnelAverage() {

}

glm::vec3 MicrofacetBRDF::getValueFromEavgTexture() {

}

glm::vec3 MicrofacetBRDF::getValueFromMicroModelBrdfTexture(const float angle, const glm::vec3 fresnel) {

}

}