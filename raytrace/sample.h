#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include "common.hpp"
#include "onb.h"

namespace CG {

class BaseSample {
public:
    virtual vec3 generateRandomSample(vec3 startPoint) = 0;

    virtual double getRandomSamplePdf(vec3 startPoint, vec3 endPoint) = 0;
private:
};

class HemisphereCosSample : public BaseSample {
public:
    HemisphereCosSample(vec3 point, vec3 normal);

    virtual vec3 generateRandomSample(vec3 startPoint) override;

    virtual double getRandomSamplePdf(vec3 startPoint, vec3 endPoint) override;
private:
    vec3 point_;
    vec3 normal_;
    onb onb_;
};

}

#endif