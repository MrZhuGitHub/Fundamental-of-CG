#include "ray.h"
#include "object.h"
#include "bvh.h"

namespace CG {
ray::ray(vec3 origin = vec3(0, 0, 0), vec3 direction = vec3(0, 0, 0))
    : origin_(origin)
    , direction_(direction)
{

}

ray::ray(ray& r) {
    direction_ = r.direction_;
    origin_ = r.origin_;
}

ray& ray::operator=(ray& r) {
    direction_ = r.direction_;
    origin_ = r.origin_;
    return *this;
}

color ray::getRayColor(std::shared_ptr<bvh> rootBvh, unsigned int launchCount) {
    if (launchCount <= 0) {
        return color(0, 0, 0);
    }

    std::vector<std::shared_ptr<object>> objects;
    rootBvh->hit(shared_from_this(), objects);

    double finalIntersection = DOUBLE_MAX;
    auto finalScatter = std::make_shared<ray>();
    vec3 finalAttenuation;
    bool isHit = false;
    for (auto& obj : objects) {
        double intersection;
        auto scatter = std::make_shared<ray>();
        vec3 attenuation;
        if (obj->hit(shared_from_this(), intersection, scatter, attenuation)) {
            if (intersection < finalIntersection) {
                finalIntersection = intersection;
                finalScatter = scatter;
                finalAttenuation = attenuation;
                isHit = true;
            }
        }
    }

    if (isHit) {
        color ret = finalAttenuation * finalScatter->getRayColor(rootBvh, launchCount - 1);
        return ret;
    } else {
        vec3 unit = direction_.unit();
        double r = 0.5*(unit.yPosition + 1);
        color ret = (1-r)*color(1, 1, 1) + r*color(0.5, 0.7, 1.0);
        return ret;
    }
}

vec3 ray::getOrigin() { return origin_; };

vec3 ray::getDirection() { return direction_.unit(); };

vec3 ray::getPoint(double interval) {
    return vec3(origin_.xPosition + interval*direction_.unit().xPosition,
                origin_.yPosition + interval*direction_.unit().yPosition,
                origin_.zPosition + interval*direction_.unit().zPosition);
}  

void ray::reset(vec3 origin, vec3 direction) {
    origin_ = origin;
    direction_ = direction;
}
}