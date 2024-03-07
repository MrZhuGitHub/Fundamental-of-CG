#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "common.hpp"
#include "bvh.h"

#include <memory>
#include <functional>

namespace CG {

using getTextureCoordinate = std::function<bool(const vec3&, float&, float&)>;

class material;
class ray;

class object : public std::enable_shared_from_this<object> {
public:
    object(std::shared_ptr<material> material);

    virtual bool hit(std::shared_ptr<ray> rayTrace, double& t, std::shared_ptr<ray> scatter, vec3& attenuation) = 0;

    virtual std::shared_ptr<aabb> getAabb() = 0;

    void setGetTextureCoordinate(getTextureCoordinate func) {
        getTextureCoordinateFunc_ = func;
        textureCoordinateValid_ = true;
    }

protected:
    std::shared_ptr<material> material_;

    getTextureCoordinate getTextureCoordinateFunc_;

    bool textureCoordinateValid_;
};

class sphere : public object {
public:
    sphere(std::shared_ptr<material> material, vec3 center, double radius);

    bool hit(std::shared_ptr<ray> rayTrace, double& t, std::shared_ptr<ray> scatter, vec3& attenuation) override;

    std::shared_ptr<aabb> getAabb() override;

    static bool getSphereTextureCoordinate(const vec3& point, float& u, float& v);

private:
    vec3 center_;
    double radius_;
}; 

class triangle : public object {
public:
    triangle(std::shared_ptr<material> material, vec3 point1, vec3 point2, vec3 point3);

    bool hit(std::shared_ptr<ray> rayTrace, double& t, std::shared_ptr<ray> scatter, vec3& attenuation) override;

    std::shared_ptr<aabb> getAabb() override;

    static bool getBarycentricCoordinate(const vec3& triangleP1, const vec3& triangleP2, const vec3& triangleP3, vec3& result);

protected:
    vec3 point1_;
    vec3 point2_;
    vec3 point3_;
    vec3 direction_;
};

class parallelogram : public object {
public:
    parallelogram(std::shared_ptr<material> material, vec3 point1, vec3 point2, vec3 point3);

    bool hit(std::shared_ptr<ray> rayTrace, double& t, std::shared_ptr<ray> scatter, vec3& attenuation) override;

    std::shared_ptr<aabb> getAabb() override;

    vec3 getPoint(float u, float v);    

protected:
    vec3 point1_;
    vec3 point2_;
    vec3 point3_;
    vec3 point4_;
    vec3 baseVec1_;
    vec3 baseVec2_;
    vec3 direction_;  
};

}

#endif