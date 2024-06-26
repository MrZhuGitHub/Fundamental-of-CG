#ifndef _RAY_H_
#define _RAY_H_

#include "common.hpp"

#include <memory>
#include <vector>

namespace CG {

class object;

class bvh;

#define DOUBLE_MAX 1000000

class ray : public std::enable_shared_from_this<ray> {
public:
    ray(vec3 origin, vec3 direction);

    ray(ray& r);

    ray& operator=(ray& r);

    color getRayColor(std::shared_ptr<bvh> rootBvh, unsigned int launchCount);

    vec3 getOrigin();

    vec3 getDirection();

    vec3 getPoint(double interval);

    void reset(vec3 origin, vec3 direction);

    void setValid(bool config) {
        valid_ = config;
    }

    bool getValid() const {
        return valid_;
    }

    static void setSky(bool sky) {
        kSky_ = sky;
    }

private:
    vec3 origin_;
    vec3 direction_;
    color background_;
    bool valid_;
    static bool kSky_;
};

}

#endif