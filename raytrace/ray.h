#ifndef _RAY_H_
#define _RAY_H_

#include "common.hpp"

#include <memory>
#include <vector>

namespace CG {

class object;

#define DOUBLE_MAX 1000000

class ray : public std::enable_shared_from_this<ray> {
public:
    ray(vec3 origin, vec3 direction);

    ray(ray& r);

    ray& operator=(ray& r);

    color getRayColor(std::vector<std::shared_ptr<object>> objects, unsigned int launchCount);

    vec3 getOrigin();

    vec3 getDirection();

    vec3 getPoint(double interval);

    void reset(vec3 origin, vec3 direction);

private:
    vec3 origin_;
    vec3 direction_;
};

}

#endif