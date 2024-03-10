#include "onb.h"

namespace CG {

onb::onb(vec3 point, vec3 normal)
    : point_(point)
    , normal_(normal) {
    zBase_ = normal_.unit();
    vec3 z1 = zBase_.xPosition > 0.8 ? vec3(0, 1, 0) : vec3(1, 0, 0);
    xBase_ = vec3::cross(zBase_, z1).unit();
    yBase_ = vec3::cross(zBase_, zBase_).unit();
}

vec3 onb::local(double x, double y, double z) {
    vec3 result = point_ + x * xBase_ + y * yBase_ + z * zBase_;
    return result;
}

}