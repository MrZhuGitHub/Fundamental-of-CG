#include "material.h"
#include "ray.h"
#include "camera.h"
#include "texture.h"
#include "object.h"
#include "sample.h"

namespace CG {
material::material(vec3 attenuation, std::shared_ptr<texture> texture)
    : attenuation_(attenuation)
    , texture_(texture)
{

}

vec3 material::GernerateRandomDirectionForSphere() {
    while(true) {
        double x = (rand()%20000)/10000.0 - 1;
        double y = (rand()%20000)/10000.0 - 1;
        double z = (rand()%20000)/10000.0 - 1;
        vec3 randomDirection(x, y, z);
        if (randomDirection.length() < 1) {
            return randomDirection.unit();
        }
    }
}

lambertian::lambertian(vec3 attenuation, std::shared_ptr<texture> texture, double albedo)
    : material(attenuation, texture)
    , albedo_(albedo) {

}

bool lambertian::scatter(std::shared_ptr<ray> in, const double& interval, const vec3& normal, std::shared_ptr<ray> out, vec3& attenuation, HitFace face, unsigned int depth, float u, float v) {
    if (texture_) {
        attenuation = texture_->lookup(u, v);
    } else {
        attenuation = attenuation_;
    }

    vec3 startPoint = in->getPoint(interval);

    double sampleScatter;
    double lambertianSamplePdf, lightSamplePdf;

    double lambertianBrdf = (1.0/PI);

    if (0 == (rand()%2)) {
        //light sample
        vec3 endPoint = mixtureVec(startPoint);
        lightSamplePdf = mixturePdf(startPoint, endPoint, normal.unit());

        // double cosAngle = vec3::dot((endPoint - startPoint).unit(), normal.unit());
        // sampleScatter = (cosAngle * lambertianBrdf)/pow((endPoint - startPoint).length(), 2);

        HemisphereCosSample hemisphereCosSample(startPoint, normal.unit());
        lambertianSamplePdf = hemisphereCosSample.getRandomSamplePdf(startPoint, endPoint);

        out->reset(in->getPoint(interval), (endPoint - startPoint).unit());
    } else {
        //lambertian sample
        HemisphereCosSample hemisphereCosSample(startPoint, normal.unit());
        vec3 endPoint = hemisphereCosSample.generateRandomSample(startPoint);
        lambertianSamplePdf = hemisphereCosSample.getRandomSamplePdf(startPoint, endPoint);
        // double cosAngle = vec3::dot((endPoint - startPoint).unit(), normal.unit());
        // sampleScatter = cosAngle * lambertianBrdf;

        lightSamplePdf = mixturePdf(startPoint, endPoint, normal.unit());

        out->reset(in->getPoint(interval), (endPoint - startPoint).unit());
    }

    double mixturePdf = 0.5 * lambertianSamplePdf + 0.5 * lightSamplePdf;
    // attenuation = (albedo_*sampleScatter*attenuation)/mixturePdf;
    if (mixturePdf > 0) {
        attenuation = (albedo_*attenuation)/mixturePdf;
        return true;
    } else {
        return false;
    }
}

double lambertian::mixturePdf(vec3 startPoint, vec3 endPoint, vec3 normal) {
    double pdfs = 0;
    double lightObjectNum = (double)lightObjects_.size();
    for (auto& lightObject : lightObjects_) {
        pdfs = pdfs + lightObject->getRandomSamplePdf(startPoint, endPoint, normal);
    }
    return (pdfs/lightObjectNum);
}

vec3 lambertian::mixtureVec(vec3 startPoint) {
    int objectNum = lightObjects_.size();
    int index = rand()%objectNum;
    return lightObjects_[index]->generateRandomSample(startPoint);
}

metal::metal(vec3 attenuation, double fuzzy, std::shared_ptr<texture> texture)
    : material(attenuation, texture)
    , fuzzy_(fuzzy) {
        fuzzy_ = fuzzy < 1 ? fuzzy : 1;
}

bool metal::scatter(std::shared_ptr<ray> in, const double& interval, const vec3& normal, std::shared_ptr<ray> out, vec3& attenuation, HitFace face, unsigned int depth, float u, float v) {
    if (texture_) {
        attenuation = texture_->lookup(u, v);
    } else {
        attenuation = attenuation_;
    }
    vec3 reflect = in->getDirection() - 2*vec3::dot(in->getDirection(), normal)*normal;
    vec3 randomDirection = GernerateRandomDirectionForSphere();
    reflect = reflect.unit() + fuzzy_*randomDirection;
    if (vec3::dot(reflect, normal) <= 0) {
        return false;
    }
    out->reset(in->getPoint(interval), reflect.unit());
    return true;
}

dielectric::dielectric(vec3 attenuation, double refraction, std::shared_ptr<texture> texture)
    : material(attenuation, texture),
        refraction_(refraction) {

}

bool dielectric::scatter(std::shared_ptr<ray> in, const double& interval, const vec3& normal, std::shared_ptr<ray> out, vec3& attenuation, HitFace face, unsigned int depth, float u, float v) {
    double ratioOfRefraction = 0;
    if (face = HitFace::HIT_FACE_OUTSIDE) {
        ratioOfRefraction = 1.0/refraction_;
    } else {
        ratioOfRefraction = refraction_/1.0;
    }

    double probability = 1;
    double currentFresnel = 1;

    vec3 inNormal = in->getDirection();
    vec3 faceNormal = -normal;
    double cosIn = vec3::dot(inNormal, faceNormal);

    if ((sqrt(1.0 - cosIn*cosIn)*ratioOfRefraction) > 1) {
        //reflect
        vec3 reflect = in->getDirection() - 2*vec3::dot(in->getDirection(), normal)*normal;
        out->reset(in->getPoint(interval), reflect); 
    } else {
        /*
        From a physical perspective, current would generate reflect ray and refraction ray,
        which would result to massive computing power. In practice, if depth <=2, it would 
        generate reflect ray and refraction ray, which is implement in smallpt. However, 
        due to software architecture limitations, this feature is temporarily not supported.
        */
        currentFresnel = fresnel(cosIn, ratioOfRefraction);
        probability = (double)(rand()%10000)/10000.0; // Russian gambling algorithm
        if (currentFresnel > probability) {
            //reflect
            vec3 reflect = in->getDirection() - 2*vec3::dot(in->getDirection(), normal)*normal;
            out->reset(in->getPoint(interval), reflect);
            probability = 1 - probability;
        } else {
            //refraction
            currentFresnel = 1 - currentFresnel;
            vec3 horizontal =  ratioOfRefraction*(inNormal + normal*cosIn);
            vec3 vertical = sqrt(1.0 - pow(horizontal.length(), 2))*faceNormal;
            vec3 refractDirection = vertical + horizontal;
            out->reset(in->getPoint(interval), refractDirection);            
        }
    }
    
    if (texture_) {
        attenuation = texture_->lookup(u, v);
    } else {
        attenuation = attenuation_;
    }

    attenuation = currentFresnel * attenuation / probability;
    
    return true;
}

double dielectric::fresnel(double cos, double refraction) {
    double R0 = pow(((refraction - 1.0)/(refraction + 1.0)), 2);
    double R = R0 + (1.0 - R0)*pow((1.0 - cos), 5);
    return R;
}

light::light(vec3 attenuation, vec3 face, std::shared_ptr<texture> texture)
    : material(attenuation, texture)
    , face_(face) {

}

bool light::scatter(std::shared_ptr<ray> in, const double& interval, const vec3& normal, std::shared_ptr<ray> out, vec3& attenuation, HitFace face, unsigned int depth, float u, float v) {
    if (texture_) {
        attenuation = texture_->lookup(u, v);
        out->setValid(false);
    } else {
        if (vec3::dot(face_, in->getDirection()) < 0) {
            attenuation = attenuation_;
        } else {
            attenuation = vec3(0, 0, 0);
        }
        out->setValid(false);
    }
    return true;
}

}