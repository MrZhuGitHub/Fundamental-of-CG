#ifndef _ONB_H_
#define _ONB_H_
#include "common.hpp"

namespace CG {

class onb {
public:
    onb(vec3 point, vec3 normal);

    vec3 local(double x, double y, double z);

private:
    vec3 point_;
    vec3 normal_;
    vec3 xBase_;
    vec3 yBase_;
    vec3 zBase_;
};

}

#endif