#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <memory>
#include <random>
#include "math.h"
#include "common.hpp"

namespace CG {

enum HitFace{
    HIT_FACE_INSIDE = 0,
    HIT_FACE_OUTSIDE,
};

class ray;

class texture;

class object;

class material {
public:
    material(vec3 attenuation, std::shared_ptr<texture> texture = nullptr);

    virtual bool scatter(std::shared_ptr<ray> in, const double& interval, const vec3& normal, std::shared_ptr<ray> out, vec3& attenuation, HitFace face, unsigned int depth, float u = 0, float v = 0) = 0;

protected:
    vec3 GernerateRandomDirectionForSphere();

protected:
    vec3 attenuation_;

    std::shared_ptr<texture> texture_;
};

class lambertian : public material {
public:
    lambertian(vec3 attenuation, std::shared_ptr<texture> texture = nullptr, double albedo = 1);

    virtual bool scatter(std::shared_ptr<ray> in, const double& interval, const vec3& normal, std::shared_ptr<ray> out, vec3& attenuation, HitFace face, unsigned int depth, float u = 0, float v = 0) override;

    void addLightObject(std::shared_ptr<object> lightObject) {
        lightObjects_.push_back(lightObject);
    };

private:
    double mixturePdf(vec3 startPoint, vec3 endPoint);

    vec3 mixtureVec(vec3 startPoint);

private:
    double albedo_;
    std::vector<std::shared_ptr<object>> lightObjects_;
};

class metal : public material {
public:
    metal(vec3 attenuation, double fuzzy = 0, std::shared_ptr<texture> texture = nullptr);

    virtual bool scatter(std::shared_ptr<ray> in, const double& interval, const vec3& normal, std::shared_ptr<ray> out, vec3& attenuation, HitFace face, unsigned int depth, float u = 0, float v = 0) override;

private:
    double fuzzy_;
};

class dielectric : public material {
public:
    dielectric(vec3 attenuation, double refraction, std::shared_ptr<texture> texture = nullptr);

    virtual bool scatter(std::shared_ptr<ray> in, const double& interval, const vec3& normal, std::shared_ptr<ray> out, vec3& attenuation, HitFace face, unsigned int depth, float u = 0, float v = 0) override;

private:
    double fresnel(double cos, double refraction);

private:
    const double refraction_;
};

class light : public material {
public:
    light(vec3 attenuation, vec3 face = vec3(0, -1, 0), std::shared_ptr<texture> texture = nullptr);

    virtual bool scatter(std::shared_ptr<ray> in, const double& interval, const vec3& normal, std::shared_ptr<ray> out, vec3& attenuation, HitFace face, unsigned int depth, float u = 0, float v = 0) override;

private:
    vec3 face_;
};
}

#endif