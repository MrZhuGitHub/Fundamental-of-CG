#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "common.hpp"
#include "bvh.h"

#include <memory>

namespace CG {

class material;
class ray;

class object : public std::enable_shared_from_this<object> {
public:
    object(std::shared_ptr<material> material);

    virtual bool hit(std::shared_ptr<ray> rayTrace, double& t, std::shared_ptr<ray> scatter, vec3& attenuation) = 0;

    virtual std::shared_ptr<aabb> getAabb() = 0;

protected:
    std::shared_ptr<material> material_;
};

class sphere : public object {
public:
    sphere(std::shared_ptr<material> material, vec3 center, double radius);

    bool hit(std::shared_ptr<ray> rayTrace, double& t, std::shared_ptr<ray> scatter, vec3& attenuation) override;

    std::shared_ptr<aabb> getAabb() override;

private:
    vec3 center_;
    double radius_;
}; 

class ground : public object {
public:
    ground(std::shared_ptr<material> material, vec3 point, vec3 direction);

    bool hit(std::shared_ptr<ray> rayTrace, double& t, std::shared_ptr<ray> scatter, vec3& attenuation) override;

    std::shared_ptr<aabb> getAabb() override;

protected:
    vec3 point_;
    vec3 direction_;
};
}

#endif