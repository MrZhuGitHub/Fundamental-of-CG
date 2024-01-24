#include "material.h"
#include "ray.h"
#include "camera.h"

namespace CG {
material::material(vec3 attenuation)
    : attenuation_(attenuation)
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

lambertian::lambertian(vec3 attenuation)
    : material(attenuation) {

}

bool lambertian::scatter(std::shared_ptr<ray> in, const double& interval, const vec3& normal, std::shared_ptr<ray> out, vec3& attenuation, HitFace face) {
    attenuation = attenuation_;
    vec3 randomDirection = GernerateRandomDirectionForSphere();
    // do {
    //     randomDirection = GernerateRandomDirectionForSphere();
    // } while (vec3::dot(randomDirection, normal) <= 0);
    vec3 direction = normal.unit() + randomDirection;
    vec3 origin = in->getPoint(interval);
    out->reset(origin, direction.unit());
    return true;
}

metal::metal(vec3 attenuation, double fuzzy)
    : material(attenuation)
    , fuzzy_(fuzzy) {
        fuzzy_ = fuzzy < 1 ? fuzzy : 1;
}

bool metal::scatter(std::shared_ptr<ray> in, const double& interval, const vec3& normal, std::shared_ptr<ray> out, vec3& attenuation, HitFace face) {
    attenuation = attenuation_;
    vec3 reflect = in->getDirection() - 2*vec3::dot(in->getDirection(), normal)*normal;
    vec3 randomDirection = GernerateRandomDirectionForSphere();
    reflect = reflect.unit() + fuzzy_*randomDirection;
    if (vec3::dot(reflect, normal) <= 0) {
        return false;
    }
    out->reset(in->getPoint(interval), reflect.unit());
    return true;
}

dielectric::dielectric(vec3 attenuation, double refraction)
    : material(attenuation),
        refraction_(refraction) {

}

bool dielectric::scatter(std::shared_ptr<ray> in, const double& interval, const vec3& normal, std::shared_ptr<ray> out, vec3& attenuation, HitFace face) {
    double ratioOfRefraction = 0;
    if (face = HitFace::HIT_FACE_OUTSIDE) {
        ratioOfRefraction = 1.0/refraction_;
    } else {
        ratioOfRefraction = refraction_/1.0;
    }

    vec3 inNormal = in->getDirection();
    vec3 faceNormal = -normal;
    double cosIn = vec3::dot(inNormal, faceNormal);

    // if ((sqrt(1-cosIn*cosIn)*ratioOfRefraction) > 1 || fresnel(cosIn, ratioOfRefraction) > ((double)(rand()%10000)/10000.0)) {
    if ((sqrt(1.0 - cosIn*cosIn)*ratioOfRefraction) > 1) {
        vec3 reflect = in->getDirection() - 2*vec3::dot(in->getDirection(), normal)*normal;
        out->reset(in->getPoint(interval), reflect); 
    } else {
        vec3 horizontal =  ratioOfRefraction*(inNormal + normal*cosIn);
        vec3 vertical = sqrt(1.0 - pow(horizontal.length(), 2))*faceNormal;
        vec3 refractDirection = vertical + horizontal;
        out->reset(in->getPoint(interval), refractDirection);
    }
    attenuation = attenuation_;
    
    return true;
}

double dielectric::fresnel(double cos, double refraction) {
    double R0 = pow(((refraction - 1.0)/(refraction + 1.0)), 2);
    double R = R0 + (1.0 - R0)*pow((1.0 - cos), 5);
    return R;
}

}